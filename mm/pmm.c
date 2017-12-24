/*
filename: 	pmm.c
author:	  	wei-coder
date:		2017-12
purpose:	物理内存管理相关内容
*/


#include "pmm.h"
#include "multiboot.h"


/*管理空闲页的链表*/
list_t	free_mem = {0};	

/*页目录表和页表的定义*/
u32		pdt[PDT_LEN];

void init_list(list_t* plist)
{
	plist->head = NULL;
	plist->tail = NULL;
}

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

void pmm_free(void* pMem, u32 len)
{
	
	free_mem.head->base = pMem;
	free_mem.head->len	= len;
	free_mem.head->next = NULL;
	free_mem.tail = free_mem.head;
}

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
	mmap_entry_t mmap_entry = glb_mboot_ptr->mmap_addr;
	u32 mmap_length = glb_mboot_ptr->mmap_length;

	while(mmap_length-- )
	{	
		/*type = 1表示是可用物理内存，从0x100000开始的内存为内核加载的内存
		0x0~0x100000之间的内存空间是bootloader的加载位置，暂时不用*/
		if((mmap_entry->type == 1) && (mmap_entry->base_addr_low == 0x100000))
		{
			/*PDT放在内核之后的内存*/
			pdt = (u32*)(mmap_entry->base_addr_low + (u32)(kern_end - kern_start));

			/*PTE放在PDT之后的内存*/
			u32* pte = (u32*)(pdt + 4*PDT_LEN);

			/*页面管理的真实物理地址应该从PTE表之后开始
			PTE占用的总空间为:PTE表个数*PTE大小*PTE表项大小*/
			u32 page_addr = pte + (PAGE_COUNT/PTE_LEN)*PTE_LEN*4;

			/*真实用户分页的内存大小为从PTE表之后到物理内存最高地址*/
			u32 max_addr = mmap_entry->base_addr_low + mmap_entry->length_low;
			u32 page_count = (max_addr-page_addr)/PMM_PAGE_SIZE;
			u32 pte_count = page_count/PTE_LEN;
			u32 remain = page_count%PTE_LEN;
			int i = 0;

			while(page_addr < max_addr)
			{
				if((PTE_LEN == i) ||(0 == i))
				{
					pdt[i/PTE_LEN] = (u32)pte+i;
				}
				pte[i] = page_addr;
				page_addr += PMM_PAGE_SIZE;
				i++;
			}
		}
		mmap_entry++;
	}

	u32 cr0;
	// 启用分页，将 cr0 寄存器的分页位置为 1 就好
	asm volatile ("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0" : : "r" (cr0));
}
