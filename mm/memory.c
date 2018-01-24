#include "memory.h"
#include "multiboot.h"
#include "console.h"
#include "logging.h"
#include "pm.h"
#include "kern_debug.h"
#include "string.h"
#include "heap.h"

// �ں�δ������ҳ����ǰ�ݴ��ָ��
multiboot_t *glb_mboot_ptr;

/*ҳĿ¼���ҳ��Ķ���*/
pdt_t*		pdt;

/*ʹ��linux0.11�ķ����������ֽ�����������ڴ�ռ��*/
static u8* mem_map = NULL;

/*�߶��ڴ��ӳ���*/
static u8* high_map = NULL;

/*��¼ʵ�ʹ��������ҳ����*/
u32 page_count = 0;

/*��¼ʵ�ʹ��������ҳ����ߵ�ַ*/
u32 end_addr = 0;

/*��¼ʵ�ʹ�������ҳ����ʼ��ַ*/
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
	/*grub̽����������ڴ汣����mmap_entry_t���У�����׵�ַΪmultiboot�ṹ���mmap_addr
	�ṹ�壬�������Ϊmmap_length.  �˴�Ҫ����mmap_entry_t��������ȡ�������ڴ����Ϣ
	��ҳ����г�ʼ����*/
	mmap_entry_t* mmap_entry = (mmap_entry_t*)(glb_mboot_ptr->mmap_addr);
	u32 mmap_length = glb_mboot_ptr->mmap_length;
	int i = 0;

	while(mmap_length-- )
	{	
		/*type = 1��ʾ�ǿ��������ڴ�*/
		if((mmap_entry->type == 1) && (kern_start == mmap_entry->base_addr_low))
		{
			/*����ҳ����*/
			page_count = mmap_entry->length_low/PAGE_SIZE;
			/*mem_mapռ�õ�����ҳ����*/
			int map_size = page_count/PAGE_SIZE;
			if(page_count%PAGE_SIZE)
			{
				map_size++;
			}
			mem_map = (u8*)kern_end;
			/*�ں˼�mem_mapռ�õĿռ���ΪUSED*/
			for( ; i<(kern_end- kern_start)/PAGE_SIZE + map_size; i++)
			{
				mem_map[i] = USED;
			}
			start_addr = (u32)(&mem_map[i+1]);
			for(; i<page_count; i++)
			{
				/*�ں�ӳ��֮��������ڴ棬���ΪFREE*/
				mem_map[i] = FREE;
			}
		}
		mmap_entry++;
	}
}

/*�˺�������ҳ�������ַ�ͷ�һ������ҳ*/
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

/*�˺�������һ������ҳ�������ַ*/
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
	pte_t* pte = NULL;

	/*��pdt��������ҳ��*/
	pdt = alloc_page() + PAGE_OFFSET;
	
	/*���߶��ڴ�һ�µ�����ҳӳ�䵽���Կռ�*/
	u32 kern_page_count = HIGH_MEM_ADDR/PAGE_SIZE;
	int pte_cont = page_count/PTE_LEN;
	if(page_count%PTE_LEN)
	{
		pte_cont++;
	}
	
	for(i=PDT_INDEX(PAGE_OFFSET); i < PDT_INDEX(PAGE_OFFSET) + pte_cont; i++)
	{
		pte = alloc_page();
		/*���ں˿ռ�ӳ�䵽0xC000 0000 ~0xFFFF FFFF*/
		pdt[i] = pte | PDT_FLAG;
	}

	for(i = 1; i<kern_page_count; i++)
	{
		/*�������ַ0x0~0x38000000ӳ�䵽0xC000 0000 ~0xF800 0000*/
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
		/*ҳ��Ϊ�գ�������һ������ҳ��Ϊҳ��*/
		pte = (pte_t *)alloc_page();
		/*ʹҳĿ¼ָ��ҳ��*/
		pdt_now[pdt_idx] = (pdt_t)((u32)pte | PDT_FLAG);

		memset((void*)((u32)pte + PAGE_OFFSET), 0, PAGE_SIZE);
	}
	else
	{
		// ת�����ں����Ե�ַ
		pte = (pte_t *)((u32)pte + PAGE_OFFSET);
	}

	pte[pte_idx] = (pte_t)((pa & PAGE_MASK) | flags);
	// ֪ͨ CPU ����ҳ����
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

	// ת�����ں����Ե�ַ
	pte = (pte_t *)((u32)pte + PAGE_OFFSET);

	pte[pte_idx] = (pte_t)0;

	// ֪ͨ CPU ����ҳ����
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
	
	// ת�����ں����Ե�ַ
	pte = (pte_t *)((u32)pte + PAGE_OFFSET);

	// �����ַ��Ч����ָ�벻ΪNULL���򷵻ص�ַ
	if ((u32)pte[pte_idx] != 0 && pa)
	{
		*pa = (u32)pte[pte_idx] & PAGE_MASK;
		return 1;
	}

	return 0;
}

#if 1

void get_empty_page(ulong address)
{
	u32 page = (u32)alloc_page();
	map(pdt,address,page,PAGE_FLAG);
}

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
