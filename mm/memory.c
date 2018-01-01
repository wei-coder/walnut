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
u32*		pdt;
u32*		pte;

/*ʹ��linux0.11�ķ����������ֽ�����������ڴ�ռ��*/
static unsigned char mem_map[PAGE_COUNT] = {0};

/*��¼ʵ�ʹ��������ҳ����*/
u32 page_count = 0;

/*��¼ʵ�ʹ��������ҳ����ߵ�ַ*/
u32 end_addr = 0;

/*��¼ʵ�ʹ��������ҳ����ʼ��ַ*/
u32 start_addr = 0;

static void page_fault(int_cont_t * context);
/* ����������������ƻ�ʵ�ֻ���㷨����ʱ����
void l_insert(list_t* plist, node_t* pNode)
{
	if (NULL == plist)
	{
		return;
	};
	if (NULL == plist->head)
	{
		pNode->next = NULL;
		pNode->prev = NULL;
		plist->head = pNode;
		plist->tail = pNode;
	}
	else
	{
		pNode->next = NULL;
		pNode->prev = plist->tail;
		plist->tail->next = pNode;
		plist->tail = pNode;
	};
};

void l_pop(list_t* plist)
{
	node_t* tmpNode = plist->head;
	plist->head = plist->head->next;
	if (NULL != plist->head)
	{
		plist->head->prev = NULL;
	};
};
*/

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

	while(mmap_length-- )
	{	
		/*type = 1��ʾ�ǿ��������ڴ棬��0x100000��ʼ���ڴ�Ϊ�ں˼��ص��ڴ�
		0x0~0x100000֮����ڴ�ռ���bootloader�ļ���λ�ã���ʱ����*/
		if((mmap_entry->type == 1) && (mmap_entry->base_addr_low == 0x100000))
		{
			/*��ʵ�û���ҳ���ڴ��СΪ��PTE��֮�������ڴ���ߵ�ַ���˵�ַΪ�����ַ*/
			end_addr = mmap_entry->base_addr_low + mmap_entry->length_low;
			
			/*����ҳ�ĸ����͵����ڴ泤�ȳ�������ҳ��С*/
			page_count = mmap_entry->length_low/PMM_PAGE_SIZE;

			/*PDT�����ں�֮����ڴ棬�˵�ַ�������ַ*/
			pdt = (u32*)(mmap_entry->base_addr_low + (u32)(kern_end - kern_start) + PAGE_OFFSET);
			
			/*PTE����PDT֮����ڴ棬�˵�ַ�������ַ*/
			pte = (u32*)((u32)pdt + 4*PDT_LEN);

			/*���PTE��ĸ�������֤4096�ֽڶ���*/
			u32 pte_count = page_count/PTE_LEN;
			if(page_count%PTE_LEN)
			{
				pte_count++;
			}
			/*�ں˹���������ڴ��ҳ�������ʼ���˵�ַΪ�����ַ*/
			start_addr = (u32)pte + pte_count*PTE_LEN*4 - PAGE_OFFSET;

			/*PDT��PTE����*/
			memset(pdt,0, PDT_LEN*4);
			memset(pte,0, pte_count*PTE_LEN*4);

			int i = 0;
			for( ; i<(end_addr - start_addr)/PMM_PAGE_SIZE; i++)
			{
				/*�ں˹���������ڴ���ں˽�����ַ��ʼ����start_addr��¼����������ڴ���end_addr��¼*/
				mem_map[i] = FREE;
			}
		}
		mmap_entry++;
	}
	
	register_int_handler(14, &page_fault);
}

/*�˺�������ҳ�������ַ�ͷ�һ������ҳ*/
void free_page(u32 addr )
{
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
			ret = start_addr + i*PMM_PAGE_SIZE;
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

	printf("Page fault at 0x%x, virtual faulting address 0x%x\n", context->eip,cr2);
	printf("Error code: %x\n", context->err_code);

	if ( !(context->err_code & 0x1))
	{
		show_string_color("Because the page wasn't present.\n", black, red);
	}

	if (context->err_code & 0x2) 
	{
		show_string_color("Write error.\n", black, red);
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
	
	/*�����ں˼�ҳĿ¼���ҳ�������ҳӳ�䵽���Կռ�*/
	u32 kern_page_count = start_addr/PMM_PAGE_SIZE;
	
	for(i=PDT_INDEX(PAGE_OFFSET); i < PDT_INDEX(PAGE_OFFSET) + page_count/PTE_LEN; i++)
	{
		/*���ں˿ռ�ӳ�䵽0xC000 0000 ~0xFFFF FFFF*/
		pdt[i] = ((u32)pte + PTE_LEN*j*4 - PAGE_OFFSET) | PDT_FLAG;
		j++;
	}

	//printf("0xE0000000's pte addr = 0x%08X; pte = 0x%08X\n", pdt[PDT_INDEX(0xE0000000)], pte[PTE_INDEX(0xE0000000)]);
	//printf("pdt start---addr=0x%08X: pdt[%d] = 0x%08X. pdt end---addr=0x%08X: pdt[%d] = 0x%08X\n", &pdt[PDT_INDEX(PAGE_OFFSET)], PDT_INDEX(PAGE_OFFSET), pdt[PDT_INDEX(PAGE_OFFSET)], &pdt[i-1],i-1, pdt[i-1]);

	for(i = 1; i<kern_page_count; i++)
	{
		pte[i] =  (i << 12) | PAGE_FLAG;
	}
	//printf("PTE start---addr=0x%08X: pte[1] = 0x%08X. PTE end---addr=0x%08X: pte[%d] = 0x%08X\n", &pte[1], pte[1], &pte[i-1], i-1, pte[i-1]);
	//printf("page count = %d\n", kern_page_count);
	//printf("KERNEL heap start PDT---addr = 0x%08X: pdt[%d] = 0x%08X; start PTE--addr=0x%08X : pte[%d] = 0x%08X\n",&pdt[PDT_INDEX(HEAP_START)], PDT_INDEX(HEAP_START),pdt[PDT_INDEX(HEAP_START)], &pte[PTE_INDEX(HEAP_START)], PTE_INDEX(HEAP_START), pte[PTE_INDEX(HEAP_START)]);
	//printf("low addr = 0x%08X; high addr = 0x%08X\n", start_addr, end_addr);
	//printf("kmalloc addr = 0x%08X\n", (u32)kmalloc);
	
	register_int_handler(14, &page_fault);
	
	u32 pdt_phy_addr = (u32)pdt - PAGE_OFFSET;
	asm volatile ("mov %0, %%cr3" : : "r" (pdt_phy_addr));
};

void map(pdt_t* pdt_now, u32 va, u32 pa, u32 flags)
{ 	
	u32 pdt_idx = PDT_INDEX(va);
	u32 pte_idx = PTE_INDEX(va); 
	
	pte_t *pte = (pte_t *)((u32)pdt_now[pdt_idx] & PAGE_MASK);
	//printf("MAP pte phy addr = 0x%08X, flage = %d\n", (u32)pte, flags);
	//printf("new flags = %d\n", flags);
	
	if (!pte)
	{
		/*ҳ��Ϊ�գ�������һ������ҳ��Ϊҳ��*/
		pte = (pte_t *)alloc_page();
		/*ʹҳĿ¼ָ��ҳ��*/
		pdt_now[pdt_idx] = (pdt_t)((u32)pte | PDT_FLAG);

		memset((void*)((u32)pte + PAGE_OFFSET), 0, PMM_PAGE_SIZE);
	}
	else
	{
		// ת�����ں����Ե�ַ
		pte = (pte_t *)((u32)pte + PAGE_OFFSET);
	}

	pte[pte_idx] = (pte_t)((pa & PAGE_MASK) | flags);
	//printf("MAP va = 0x%08X; pa = 0x%08X, pte[%d]=0x%08X\n", va, pa, pte_idx, pte[pte_idx]);
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
	printf("UNMAP va = 0x%08X; pte[%d]=0x%08X\n", va, pte_idx, pte[pte_idx]);

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


