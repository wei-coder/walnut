/*
filename:	hardware.c
author:	wei-code
date:	2018-11
prupose:系统硬件初始化相关函数
*/

#include "../mm/heap.h"
#include "../driver/io.h"
#include "pci.h"

struct pci_info g_pciinfo = {0};


void init_pci()
{
	u32 value = 0;
	for(int i=0;i<256; i++)
	{
		for(int j=0; j<32; j++)
		{
			for(int n=0; n<8; n++)
			{
				struct pci_device * tmpdev = (struct pci_device *)kmalloc(sizeof(struct pci_device));
				tmpdev->devcaddr.dvcd_enable = 0x1;
				tmpdev->devcaddr.dvcd_reserve = 0x0;
				tmpdev->devcaddr.dvcd_busno = i;
				tmpdev->devcaddr.dvcd_devno = j;
				tmpdev->devcaddr.dvcd_funcno = n;
				value = *((u32 *)(&tmpdev->devcaddr));
				outl_p(value,PCI_CONFIG_ADDRESS);
				ulong * pret = (ulong *)tmpdev;
				for(int m=0; m<16; m++)
				{
					pret[m] = inl(PCI_CONFIG_DATA);
					if(0xFFFFFFFF == pret[m])
					{
						break;
					}
				}
				if(pret[0] != 0xFFFFFFFF)
				{
					//如果没有返回错误，则为有效信息，将其加入设备链表
					if(NULL == g_pciinfo.devlist)
					{
						tmpdev->devNode.next = &(tmpdev->devNode);
						tmpdev->devNode.prev = &(tmpdev->devNode);
						g_pciinfo.devlist = tmpdev;
					}
					else
					{
						push_ring_list(&(g_pciinfo.devlist->devNode),&(tmpdev->devNode));
					}
				}
				else
				{
					kfree(tmpdev);
				}
			}
		}
	}
}
