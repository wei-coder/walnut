#include "memory.h"
#include "multiboot.h"
#include "console.h"
#include "logging.h"
#include "pm.h"
#include "kern_debug.h"
#include "string.h"
#include "heap.h"

// 内核未建立分页机制前暂存的指针
multiboot_t *glb_mboot_ptr;

/*页目录表和页表的定义*/
u32*		pdt;
u32*		pte;

/*使用linux0.11的方案，采用字节数组来标记内存占用*/
static unsigned char mem_map[PAGE_COUNT] = {0};

/*记录实际管理的物理页数量*/
u32 page_count = 0;

/*记录实际管理的物理页的最高地址*/
u32 end_addr = 0;

/*记录实际管理的物理页的起始地址*/
u32 start_addr = 0;

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

	while(mmap_length-- )
	{	
		/*type = 1表示是可用物理内存，从0x100000开始的内存为内核加载的内存
		0x0~0x100000之间的内存空间是bootloader的加载位置，暂时不用*/
		if((mmap_entry->type == 1) && (mmap_entry->base_addr_low == 0x100000))
		{
			/*真实用户分页的内存大小为从PTE表之后到物理内存最高地址，此地址为物理地址*/
			end_addr = mmap_entry->base_addr_low + mmap_entry->length_low;
			
			/*物理页的个数就等于内存长度除以物理页大小*/
			page_count = mmap_entry->length_low/PAGE_SIZE;

			/*PDT放在内核之后的内存，此地址是物理地址*/
			pdt = (u32*)(mmap_entry->base_addr_low + (u32)(kern_end - kern_start) + PAGE_OFFSET);
			
			/*PTE放在PDT之后的内存，此地址是物理地址*/
			pte = (u32*)((u32)pdt + 4*PDT_LEN);

			/*获得PTE表的个数，保证4096字节对齐*/
			u32 pte_count = page_count/PTE_LEN;
			if(page_count%PTE_LEN)
			{
				pte_count++;
			}
			/*内核管理的物理内存从页表结束开始，此地址为物理地址*/
			start_addr = (u32)pte + pte_count*PTE_LEN*4 - PAGE_OFFSET;

			/*PDT和PTE清零*/
			memset(pdt,0, PDT_LEN*4);
			memset(pte,0, pte_count*PTE_LEN*4);

			int i = 0;
			for( ; i<(end_addr - start_addr)/PAGE_SIZE; i++)
			{
				/*内核管理的物理内存从内核结束地址开始，由start_addr记录，最高物理内存由end_addr记录*/
				mem_map[i] = FREE;
			}
		}
		mmap_entry++;
	}
	
	register_int_handler(14, &page_fault);
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
			ret = start_addr + i*PAGE_SIZE;
			mem_map[i] = USED;
			return (void *)ret;
		}
	}
	return (void *)ret;
}

void page_fault(int_cont_t * context)
{
	u32 cr2;
	asm volatile ("mov %%cr2, %0" : "=r" (cr2));

	printf("Page fault at 0x%08x, virtual faulting address 0x%08x\n", context->eip,cr2);
	printf("Error code: %x\n", context->err_code);

	if ( !(context->err_code & 0x1))
	{
		show_string_color("Because the page wasn't present.\n", black, red);
		do_no_page();
	}

	if (context->err_code & 0x2) 
	{
		show_string_color("Write error.\n", black, red);
		do_wp_page();
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
	int j = 0;
	
	/*仅将内核及页目录表和页表的物理页映射到线性空间*/
	u32 kern_page_count = start_addr/PAGE_SIZE;
	
	for(i=PDT_INDEX(PAGE_OFFSET); i < PDT_INDEX(PAGE_OFFSET) + page_count/PTE_LEN; i++)
	{
		/*将内核空间映射到0xC000 0000 ~0xFFFF FFFF*/
		pdt[i] = ((u32)pte + PTE_LEN*j*4 - PAGE_OFFSET) | PDT_FLAG;
		j++;
	}

	for(i = 1; i<kern_page_count; i++)
	{
		pte[i] =  (i << 12) | PAGE_FLAG;
	}
	
	register_int_handler(14, &page_fault);
	
	u32 pdt_phy_addr = (u32)pdt - PAGE_OFFSET;
	asm volatile ("mov %0, %%cr3" : : "r" (pdt_phy_addr));
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

#if 0
void do_no_page(ulong error_code,ulong address)
{
	int nr[4];
	ulong tmp;
	ulong page;
	int block,i;

	address &= 0xfffff000;
	tmp = address - current->start_code;
	if (!current->executable || tmp >= current->end_data)
	{
		get_empty_page(address);
		return;
	}
	if (share_page(tmp))
		return;
	if (!(page = get_free_page()))
		oom();
/* remember that 1 block is used for header */
	block = 1 + tmp/BLOCK_SIZE;
	for (i=0 ; i<4 ; block++,i++)
		nr[i] = bmap(current->executable,block);
	bread_page(page,current->executable->i_dev,nr);
	i = tmp + 4096 - current->end_data;
	tmp = page + 4096;
	while (i-- > 0)
	{
		tmp--;
		*(char *)tmp = 0;
	}
	if (put_page(page,address))
		return;
	free_page(page);
	oom();
}

void un_wp_page(ulong * table_entry)
{
	ulong old_page,new_page;

	old_page = 0xfffff000 & *table_entry;
	if (old_page >= LOW_MEM && mem_map[MAP_NR(old_page)]==1)
	{
		*table_entry |= 2;
		invalidate();
		return;
	}
	if (!(new_page=get_free_page()))
		oom();
	if (old_page >= LOW_MEM)
		mem_map[MAP_NR(old_page)]--;
	*table_entry = new_page | 7;
	invalidate();
	copy_page(old_page,new_page);
}

void do_wp_page(ulong error_code,ulong address)
{
	un_wp_page((ulong *)(((address>>10) & 0xffc) + (0xfffff000 &	*((ulong *) ((address>>20) &0xffc)))));
}

#endif
