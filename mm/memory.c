

#include "memory.h"
#include "console.h"
#include "logging.h"
#include "pm.h"
#include "kern_debug.h"
#include "string.h"
#include "heap.h"
#include "sched.h"

// 内核未建立分页机制前暂存的指针
multiboot_t *glb_mboot_ptr;

/*使用linux0.11的方案，采用字节数组来标记内存占用*/
static u8* mem_map = NULL;

/*高端内存的映射表*/
//static u8* high_map = NULL;

/*记录实际管理的物理页数量*/
u32 page_count = 0;

/*记录实际管理的物理页的最高地址*/
u32 end_addr = 0;

/*记录实际管理物理页的起始地址*/
u32 start_addr = 0;

/*内核全局页目录表*/
pdt_t* pdt;

#define invalidate() asm volatile("mov %0,%%cr3"::"r" (current->pdt))

// 复制1 页内存（4K 字节）。
#define copy_page(from,to) \
asm volatile("cld ; rep ; movsb"::"S" (from),"D" (to),"c" (4096):)

static void page_fault(int_cont_t * context);

void show_mem_map()
{
	u32 mmap_addr = glb_mboot_ptr->mmap_addr;
	u32 mmap_length = glb_mboot_ptr->mmap_length;

	printf("Memory map:\n");

	mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
	for (mmap = (mmap_entry_t *)mmap_addr; (u32)mmap < mmap_addr +mmap_length; mmap++)
	{
		printf("base_addr = 0x%X%08X, length = 0x%X%08X, type = 0x%X\n",(u32)mmap->base_addr_high , (u32)mmap->base_addr_low ,
		(u32)mmap->length_high , (u32)mmap->length_low , (u32)mmap->type);
	}
}

void init_pmm()
{
	/*grub探测出的物理内存保存在mmap_entry_t表中，表的首地址为multiboot结构体的mmap_addr
	结构体，表项个数为mmap_length.  此处要遍历mmap_entry_t表，并从中取出物理内存的信息
	对页表进行初始化。*/
	mmap_entry_t* mmap_entry = (mmap_entry_t*)(glb_mboot_ptr->mmap_addr);
	u32 mmap_length = glb_mboot_ptr->mmap_length;
	int i = 0;

	while(mmap_length-- )
	{	
		/*type = 1表示是可用物理内存*/
		if((mmap_entry->type == 1) && (NOMAL_MEM_ADDR == mmap_entry->base_addr_low))
		{
			/*mem_map放在kern_end之后*/
			mem_map = (u8*)kern_end;
			/*物理页面数*/
			page_count = mmap_entry->length_low/PAGE_SIZE;
			/*mem_map占用的物理页面数*/
			int map_size = page_count/PAGE_SIZE;
			if(page_count%PAGE_SIZE)
			{
				map_size++;
			}
			/*可分配内存从mem_map结束的地址开始，需要注意页对齐*/
			start_addr = (u32)kern_end + map_size*PAGE_SIZE;
			/*内核及mem_map占用的空间标记为USED*/
			for( ; i<(kern_end- kern_start)/PAGE_SIZE + map_size; i++)
			{
				mem_map[i] = USED;
			}

			for(; i<page_count; i++)
			{
				/*内核映像之外的物理内存，标记为FREE*/
				mem_map[i] = FREE;
			}
		}
		mmap_entry++;
	}
}

/*此函数根据页的物理地址释放一个物理页*/
void free_page(u32 addr )
{
	//printf("free page: 0x%08X\n", addr);
	if(start_addr > addr)
	{
		return;
	}
	addr -= start_addr;
	addr >>= 12;

	if(mem_map[addr]--)
	{
		return;
	};

	mem_map[addr]= FREE;
	printf("fault: trying to free free page!\n");
}

/*此函数返回一个物理页的物理地址*/
void* alloc_page()
{
	u32 ret = NULL;

	int i = 0;
	for(i = 0; i < page_count; i++)
	{
		if(FREE == mem_map[i]) 
		{
			ret = NOMAL_MEM_ADDR + i*PAGE_SIZE;
			mem_map[i] = USED;
			return (void *)ret;
		}
	}
	return (void *)ret;
}

#if 1

/*缺页异常的处理函数*/
void do_no_page(u32 error_code, u32 address)
{
	if(!(error_code & 0x1))
	{
		return;
	}
	/*页对齐*/
	address &= 0xfffff000;
	u32 page = (u32)alloc_page();
	map(pdt,address,page,PAGE_FLAG);
	return;
}

/*取消写保护处理函数*/
void un_wp_page(u32 * table_entry)
{
	u32 old_page,new_page;

	old_page = 0xfffff000 & *table_entry;
	if (old_page >= NOMAL_MEM_ADDR && mem_map[MAP_INDEX(old_page)]==USED)
	{
		*table_entry |= 2;
		invalidate();
		return;
	}
	new_page = (u32)alloc_page();
	if (!new_page)
	{
		printf("there is no free page;\n");
	}
	if (old_page >= NOMAL_MEM_ADDR)
		mem_map[MAP_INDEX(old_page)]--;
	*table_entry = new_page | 7;
	invalidate();
	copy_page(old_page,new_page);
}

/*写保护异常处理函数*/
void do_wp_page(u32 error_code,u32 address)
{
	un_wp_page((u32 *)(((pdt[PDT_INDEX(address)]) & 0xffc) + (0xfffff000 &	*((u32 *) ((address>>20) &0xffc)))));
}

void * get_virt_page()
{
	u32 page = (u32)alloc_page();
	if(NULL != page)
	{
		return (void*)(page+PAGE_OFFSET);
	}
	return NULL;
}

void free_virt_page(u32 p)
{
	free_page(p-PAGE_OFFSET);
}

#endif

void page_fault(int_cont_t * context)
{
	u32 cr2;
	asm volatile ("mov %%cr2, %0" : "=r" (cr2));

	printf("Page fault at 0x%08x, virtual faulting address 0x%08x\n", context->eip,cr2);
	printf("Error code: %x\n", context->err_code);

	if ( !(context->err_code & 0x1))
	{
		show_string_color("Because the page wasn't present.\n", black, red);
		do_no_page(context->err_code, cr2);
	}

	if (context->err_code & 0x2) 
	{
		show_string_color("Write error.\n", black, red);
		do_wp_page(context->err_code, cr2);
	}
	else
	{
		show_string_color( "Read error.\n", black, red);
	}

	if (context->err_code & 0x4)
	{
		show_string_color("In user mode.\n", black, red);
	}
	else
	{
		show_string_color("In kernel mode.\n", black, red);
	}

	if (context->err_code & 0x8)
	{
		show_string_color("Reserved bits being overwritten.\n", black, red);
	}

	if (context->err_code & 0x10)
	{
		show_string_color("The fault occurred during an instruction fetch.\n", black, red);
	}

	//panic("page fault stack:\n");
	
	while (1);
};


void init_vmm()
{
	int i = 0;
	pte_t* pte = NULL;

	/*给pdt申请物理页面*/
	pdt = alloc_page() + PAGE_OFFSET;
	if((u32)pdt == PAGE_OFFSET)
	{
		printf("alloc page fault!\n");
		return;
	}

	/*将高端内存以下的物理页映射到线性空间*/
	u32 kern_page_count = HIGH_MEM_ADDR/PAGE_SIZE;
	int pte_cont = page_count/PTE_LEN;
	if(page_count%PTE_LEN)
	{
		pte_cont++;
	}

	for(i=PDT_INDEX(PAGE_OFFSET); i < PDT_INDEX(PAGE_OFFSET) + pte_cont; i++)
	{
		pte = alloc_page();
		/*将内核空间映射到0xC000 0000 ~0xFFFF FFFF*/
		pdt[i] = (u32)pte | PDT_FLAG;
	}
	
	pte = (pte_t*)((u32)pdt + PAGE_SIZE);
	for(i = 1; i<kern_page_count; i++)
	{
		/*将物理地址0x0~0x38000000映射到0xC000 0000 ~0xF800 0000*/
		pte[i] =  (i << 12) | PAGE_FLAG;
	}

	register_int_handler(14, &page_fault);
	
	u32 pdt_phy_addr = (u32)pdt - PAGE_OFFSET;
	asm volatile ("mov %0,%%cr3" : : "r" (pdt_phy_addr));
};

void map(pdt_t* pdt_now, u32 va, u32 pa, u32 flags)
{ 	
	u32 pdt_idx = PDT_INDEX(va);
	u32 pte_idx = PTE_INDEX(va); 
	
	pte_t *pte = (pte_t *)((u32)pdt_now[pdt_idx] & PAGE_MASK);
	
	if (!pte)
	{
		/*页表为空，则申请一个物理页作为页表*/
		pte = (pte_t *)alloc_page();
		/*使页目录指向页表*/
		pdt_now[pdt_idx] = (pdt_t)((u32)pte | PDT_FLAG);

		memset((void*)((u32)pte + PAGE_OFFSET), 0, PAGE_SIZE);
	}
	else
	{
		// 转换到内核线性地址
		pte = (pte_t *)((u32)pte + PAGE_OFFSET);
	}

	pte[pte_idx] = (pte_t)((pa & PAGE_MASK) | flags);
	// 通知 CPU 更新页表缓存
	asm volatile ("invlpg (%0)" : : "a" (va));
}

void unmap(pdt_t * pdt_now, u32 va)
{
	u32 pgd_idx = PDT_INDEX(va);
	u32 pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)((u32)pdt_now[pgd_idx] & PAGE_MASK);

	if (!pte)
	{
		return;
	}

	// 转换到内核线性地址
	pte = (pte_t *)((u32)pte + PAGE_OFFSET);

	pte[pte_idx] = (pte_t)0;

	// 通知 CPU 更新页表缓存
	asm volatile ("invlpg (%0)" : : "a" (va));
}

u32 get_mapping(pdt_t *pdt_now, u32 va, u32 *pa)
{
	u32 pgd_idx = PDT_INDEX(va);
	u32 pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)((u32)pdt_now[pgd_idx] & PAGE_MASK);
	if (!pte)
	{
		return 0;
	}
	
	// 转换到内核线性地址
	pte = (pte_t *)((u32)pte + PAGE_OFFSET);

	// 如果地址有效而且指针不为NULL，则返回地址
	if ((u32)pte[pte_idx] != 0 && pa)
	{
		*pa = (u32)pte[pte_idx] & PAGE_MASK;
		return 1;
	}

	return 0;
}

int copy_page_tables (u32* from, u32* to)
{
	pte_t* pte = NULL;
	u32 from_page = 0;
	u32 to_page = 0;
	
	if((NULL == from) ||(NULL == to))
	{
		return FALSE;
	}

	//先复制PDT表以及PTE表
	int i = 0;
	for(; i < PDT_LEN; i++)
	{
		if(from[i] & 0x01)
		{
			pte = alloc_page();
			//copy父进程的page flag
			to[i] = (u32)pte | (from[i] & FLAG_MASK);
			from_page = (from[i]+PAGE_OFFSET)&PAGE_MASK;
			to_page = (to[i]+PAGE_OFFSET)&PAGE_MASK;
			copy_page(from_page, to_page);
		}
	}

	invalidate();
	return 0;
}

int free_page_tables(u32 pdt_addr)
{
	u32* proc_pdt = (u32*)pdt_addr;

	if(NULL == proc_pdt) 
	{
		return FALSE;
	}

	//释放页表
	int i = 0;
	for(; i < PDT_LEN; i++)
	{
		if(proc_pdt[i] & 0x01)
		{
			free_page(proc_pdt[i]&PAGE_MASK);
		}
	}

	free_page(pdt_addr);

	invalidate(); 
	return 0;
}
