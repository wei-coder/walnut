#include "memory.h"
#include "multiboot.h"
#include "console.h"
#include "logging.h"
#include "pm.h"

// �ں�δ������ҳ����ǰ�ݴ��ָ��
multiboot_t *glb_mboot_ptr;

/*ҳĿ¼���ҳ��Ķ���*/
u32*		pdt;

/*ʹ��linux0.11�ķ����������ֽ�����������ڴ�ռ��*/
static unsigned char mem_map[PAGE_COUNT] = {0};

/*��¼ʵ�ʹ��������ҳ����*/
u32 page_count = 0;

/*��¼ʵ�ʹ��������ҳ����ߵ�ַ*/
u32 max_addr = 0;

/*��¼ʵ�ʹ��������ҳ����ʼ��ַ*/
u32 low_addr = 0;

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
			/*ҳ��������ʵ�����ַӦ�ô��ں˼��ص�ַ����*/
			low_addr = mmap_entry->base_addr_low + (u32)(kern_end - kern_start) + PAGE_OFFSET;

			/*��ʵ�û���ҳ���ڴ��СΪ��PTE��֮�������ڴ���ߵ�ַ*/
			max_addr = mmap_entry->base_addr_low + mmap_entry->length_low + PAGE_OFFSET;
			page_count = (max_addr-low_addr)/PMM_PAGE_SIZE;

			int i = 0;
			for( i = 0; i < page_count; i++)
			{
				mem_map[i] = FREE;
			}
		}
		mmap_entry++;
	}
}

void free_page(u32 addr )
{
	if(low_addr > addr)
	{
		return;
	}
	addr -= low_addr;
	addr >>= 12;

	if(mem_map[addr]--)
	{
		return;
	};

	mem_map[addr]= FREE;
	logging("fault: trying to free free page!\n");
}

void free_mem(u32 addr, u32 size)
{
	size /= PMM_PAGE_SIZE;
	if(size%PMM_PAGE_SIZE)
	{
		size++;
	}
	
	while(size--)
	{
		free_page(addr+size*PMM_PAGE_SIZE);
	}
}

void* alloc_page(u32 size)
{
	u32 ret = NULL;
	u32 tmp = 0;

	size /= PMM_PAGE_SIZE;
	if(size%PMM_PAGE_SIZE)
	{
		size++;
	}

	int i = 0;
	for(i = 0; i < page_count; i++)
	{
		if(FREE == mem_map[i]) 
		{
			if(0 == tmp)
			{
				ret = low_addr + i*PMM_PAGE_SIZE;
			}
			tmp++;
		}
		else
		{
			tmp = 0;
			ret = 0;
		}
		if(tmp >= size)
		{
			break;
		}
	}

	if(0 == ret)
	{
		return NULL;
	}
	else
	{
		tmp = ret - low_addr;
		tmp >>= 12;
		int j = 0;
		for( j = 0; j<size; j++)
		{
			mem_map[tmp] = USED;
			tmp++;
		}
		return (void *)ret;
	}
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

	while (1);
};


void init_vmm()
{
	/*PDT�����ں�֮����ڴ�*/
	pdt = (u32*)low_addr;

	/*PTE����PDT֮����ڴ�*/
	u32* pte = (u32*)((u32)pdt + 4*PDT_LEN);

	/*ҳ��������ʵ�����ַӦ�ô�PTE��֮��ʼ
	PTEռ�õ��ܿռ�Ϊ: ҳ�ĸ���*ÿ��ҳ����Ĵ�С*/
	//u32 page_addr = (u32)pte + page_count*4;

	int i = 0;
	int j = 0;
	for(i=PAGE_INDEX(PAGE_OFFSET); i < PAGE_INDEX(PAGE_OFFSET) + page_count/PTE_LEN; i++)
	{
		pdt[i] = ((u32)pte+PTE_LEN*j*4 - PAGE_OFFSET) | PDT_FLAG;
		j++;
		//printf("pdt[%d] = %d\n", i, pdt[i]);
	}

	for(i = 1; i<page_count; i++)
	{
		pte[i] =  (i << 12) | PAGE_FLAG;
		//printf("pte[%d] = %d", i, pte[i]);
	}

	printf("page count = %d\n", page_count);
	printf("high addr page index = %d\n", PAGE_INDEX(PAGE_OFFSET));
	printf("PDT first addr = %d\n", (u32)pdt);
	printf("PTE first addr = %d\n", (u32)pte);

	printf("pte[1]=%d\n", pte[1]);
	
	//while(1);

	/*
	while(i < page_count)
	{
		if((0 == i%PTE_LEN) ||(0 == i))
		{
			pdt[PAGE_INDEX(PAGE_OFFSET) + i/PTE_LEN] = (u32)pte+i*4;
		}
		pte[i] = (i << 12) | PAGE_FLAG;
		i++;
	}
	*/
	
	register_int_handler(14, &page_fault);
	
	u32 pdt_phy_addr = (u32)pdt - PAGE_OFFSET;
	asm volatile ("mov %0, %%cr3" : : "r" (pdt_phy_addr));
};


