
#include <kio.h>
#include <string.h>
#include <system.h>
#include "../entry/kern_debug.h"
#include "../entry/trap_gate.h"
#include "../entry/pm.h"
#include "../util/logging.h"
#include "../task/sched.h"
#include "memory.h"
#include "heap.h"

// 内核未建立分页机制前暂存的指针
multiboot_t *glb_mboot_ptr;

/*使用linux0.11的方案，采用字节数组来标记内存占用*/
static u8* mem_map = NULL;


/*记录实际管理的物理页数量*/
u32 page_count = 0;

/*记录实际管理的物理页的最高地址*/
u32 end_addr = 0;

/*记录实际管理物理页的起始地址*/
u32 start_addr = 0;

/*内核全局页目录表*/
pdt_t* pdt = NULL;


#define invalidate() asm volatile("mov %0,%%cr3"::"r" (current->pdt))

// 复制1 页内存（4K 字节）。
#define copy_page(from,to) \
asm volatile("cld ; rep ; movsb"::"S" (from),"D" (to),"c" (4096):)

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
				map_size ++;
			}
			/*可分配内存从mem_map结束的地址开始，需要注意页对齐*/
			start_addr = (u32)kern_end + map_size*PAGE_SIZE;

			//预留出内核页目录表的空间。
			pdt = (pdt_t*)start_addr;
			start_addr += PAGE_SIZE;

			//预留出线性映射区对应的页表空间, 线性映射区为0xc0000000~0xF8000000,总共896M.
			start_addr += PTE_COUNT*PAGE_SIZE;
			
			/*内核及mem_map以及内核页表占用的空间标记为USED*/
			int i = 0;
			for( ; i<(kern_end- kern_start)/PAGE_SIZE + map_size + PTE_COUNT + 1; i++)
			{
				mem_map[i] = USED;
			}
			printf("there are %d page used, and start_addr is 0x%08x\n",i, start_addr);

			for(; i<page_count; i++)
			{
				/*内核映像之外的物理内存，标记为FREE*/
				mem_map[i] = FREE;
			}
		}
		mmap_entry++;
	}
	logging("init phyisc memory success!\n");
}

/*此函数根据页的物理地址释放一个物理页*/
void free_page(u32 addr )
{
	//将线性地址转化为物理地址，phy_min表示可分配物理内存的最小地址。
	u32 phy_min = start_addr - PAGE_OFFSET;
	//phy_start是可管理的物理内存的起始地址，即0x100000以上地址在管理范围内，
	//低于1M的物理内存不在管理范围内
	u32 phy_start = NOMAL_MEM_ADDR;
	
	if(phy_min > addr)
	{
		return;
	}
	addr -= phy_start;
	addr >>= 12;

	if(mem_map[addr]--)
	{
		return;
	};

	mem_map[addr] = FREE;
	printf("fault: trying to free free page!\n");
}

/*此函数返回一个物理页的物理地址*/
void* alloc_page()
{
	u32 ret = NULL;

	int i = 0;
	for(i = 0; i < page_count; i++)
	{
		if(mem_map[i] == FREE) 
		{
			ret = NOMAL_MEM_ADDR + i*PAGE_SIZE;
			mem_map[i] ++;
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
	old_page  += PAGE_OFFSET;
	if (old_page >= start_addr && mem_map[MAP_INDEX(old_page)]==1)
	{
		//如果页表项已经存在，则修改可写权限即可
		*table_entry |= 2;
		invalidate();
		return;
	}

	///如果该页的地址小于0x100000，或者该页尚未被使用
	//则申请一个物理页，并修改对应表项
	new_page = (u32)alloc_page();
	if (!new_page)
	{
		printf("there is no free page;\n");
	}
// 如果原页面大于内存低端（则意味着mem_map[]>1，页面是共享的），则将原页面的页面映射
// 数组值递减1。然后将指定页表项内容更新为新页面的地址，并置可读写等标志(U/S, R/W, P)。
// 刷新页变换高速缓冲。最后将原页面内容复制到新页面。    
	if (old_page >= start_addr)
	{
		mem_map[MAP_INDEX(old_page)]--;
	}
	new_page += PAGE_OFFSET;
	copy_page(old_page,new_page);

	new_page -= PAGE_OFFSET;
	*table_entry = new_page | 7;
	invalidate();
}

/*写保护异常处理函数*/
void do_wp_page(u32 error_code,u32 address)
{
	pdt_t* tmp_pdt = (pdt_t*)((u32)current->pdt + PAGE_OFFSET);
	pte_t* pte = (pte_t*)(tmp_pdt[PDT_INDEX(address)]);
	pte = (pte_t*)(((u32)pte & 0xFFFFF000) + PTE_INDEX(address) * 4 + PAGE_OFFSET);
	un_wp_page(pte);
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

void init_vmm()
{
	pte_t* pte = (pte_t*)((u32)pdt+PAGE_SIZE);
	/*将高端内存以下的物理页映射到线性空间*/
	int i = 0;
	int j = 0;
	int k = 0;
	for(i=KERN_PDT_INDEX; i < KERN_PDT_INDEX + PTE_COUNT; i++)
	{
		/*pte紧随PDT放置*/
		pdt[i] = ((u32)pte - PAGE_OFFSET) | PDT_FLAG;

		/*将内核空间映射到0xC000 0000 ~0xF800 0000*/
		for(j = 0; j<PTE_LEN; j++)
		{
			/*将物理地址0x0~0x38000000映射到0xC000 0000 ~0xF800 0000*/
			pte[j] =  (k << 12) | PAGE_FLAG;
			k++;
		}
		pte = (pte_t*)((u32)pte + PAGE_SIZE);
	}

	set_trap_gate(14,&page_fault);
	
	u32 pdt_phy_addr = (u32)pdt - PAGE_OFFSET;
	asm volatile ("mov %0,%%cr3" : : "r" (pdt_phy_addr));
	logging("init virtual memory success!\n");
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

int copy_page_tables (u32* from_pdt, u32* to_pdt)
{
	u32* from_pte = NULL;
	u32* to_pte = NULL;
	u32 this_page = 0;
	
	if((NULL == from_pdt) ||(NULL == to_pdt))
	{
		return FALSE;
	}

	//先复制PDT表以及PTE表
	int i = 0;
	int j = 0;
	for(i = 0; i < PDT_LEN; i++)
	{
		if(from_pdt[i] & 1)
		{
			to_pte = alloc_page();
			//copy父进程页表
			to_pdt[i] = (u32)to_pte | PAGE_FLAG;
			from_pte = (u32*)((from_pdt[i]&PAGE_MASK)+PAGE_OFFSET);
			to_pte = (u32*)((u32)to_pte + PAGE_OFFSET);
			for(j=0; j<PTE_LEN; j++)
			{
				this_page = from_pte[j];
				if(this_page & 1)
				{
					if(this_page > NOMAL_MEM_ADDR)
					{
						this_page &= ~2;
					}
					to_pte[j] = this_page;
					if(from_pte[j] > start_addr)
					{
						from_pte[j] = this_page;
						mem_map[MAP_INDEX(this_page)]++;
					}
				}
			}
		}
	}

	invalidate();
	return true;
}

int free_page_tables(u32 pdt_addr)
{
	u32* proc_pdt = (u32*)(pdt_addr + PAGE_OFFSET);

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
