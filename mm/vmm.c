/*
filename: 	vmm.c
author:	  	wei-coder
date:		2017-12
purpose:	�����ڴ�����������
*/

#include "vmm.h"

void init_vmm()
{
	/*grub̽����������ڴ汣����mmap_entry_t���У�����׵�ַΪmultiboot�ṹ���mmap_addr
	�ṹ�壬�������Ϊmmap_length.  �˴�Ҫ����mmap_entry_t��������ȡ�������ڴ����Ϣ
	��ҳ����г�ʼ����*/
	mmap_entry_t mmap_entry = glb_mboot_ptr->mmap_addr;
	u32 mmap_length = glb_mboot_ptr->mmap_length;

	while(mmap_length-- )
	{	
		/*type = 1��ʾ�ǿ��������ڴ棬��0x100000��ʼ���ڴ�Ϊ�ں˼��ص��ڴ�
		0x0~0x100000֮����ڴ�ռ���bootloader�ļ���λ�ã���ʱ����*/
		if((mmap_entry->type == 1) && (mmap_entry->base_addr_low == 0x100000))
		{
			/*PDT�����ں�֮����ڴ�*/
			pdt = (u32*)(mmap_entry->base_addr_low + (u32)(kern_end - kern_start));

			/*PTE����PDT֮����ڴ�*/
			u32* pte = (u32*)(pdt + 4*PDT_LEN);

			/*ҳ��������ʵ�����ַӦ�ô�PTE��֮��ʼ
			PTEռ�õ��ܿռ�Ϊ:PTE�����*PTE��С*PTE�����С*/
			u32 page_addr = pte + (PAGE_COUNT/PTE_LEN)*PTE_LEN*4;

			/*��ʵ�û���ҳ���ڴ��СΪ��PTE��֮�������ڴ���ߵ�ַ*/
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
	// ���÷�ҳ���� cr0 �Ĵ����ķ�ҳλ��Ϊ 1 �ͺ�
	asm volatile ("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0" : : "r" (cr0));
}

