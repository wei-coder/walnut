/*
filename:	HD.c
author:	wei-code
date:	2018-04
prupose:	硬盘驱动的相关实现
*/

#include "hd.h"
#include "io.h"
#include "pm.h"
#include "system.h"
#include "console.h"

extern void hd_interrupt();
extern ulong do_hd;

char * hdbuffer = NULL;

#define port_read(port,buf,nr) \
__asm__("cld;rep;insw"::"d" (port),"D" (buf),"c" (nr))

#define port_write(port,buf,nr) \
__asm__("cld;rep;outsw"::"d" (port),"S" (buf),"c" (nr))



void hd_cmd_excu(hd_cmd_t * cmd)
{
	//硬盘中断使能
	outb_p(cmd->ctl, HD_PRI_CONTROL);
	outb_p(cmd->feature, HD_PRI_ERR_FEAT);
	outb_p(cmd->count, HD_PRI_SEC_COUNT);
	outb_p(cmd->lba_low, HD_PRI_LBA_LOW);
	outb_p(cmd->lba_mid, HD_PRI_LBA_MID);
	outb_p(cmd->lba_high, HD_PRI_LBA_HIGH);
	outb_p(cmd->device, HD_PRI_DEVICE);
	outb_p(cmd->command, HD_PRI_STAT_COM);
	do_hd = (ulong)cmd->ptr_hd;
}

bool is_hd_ready()
{
	u8 status = inb_p(HD_PRI_CONTROL);
	if(status == 0x58)
	{
		return true;
	}
	return false;
}

int wait_hd()
{
	int retries=100000;
	while(!is_hd_ready() && --retries)
	{
		nop();
	}
	return retries;
}

void read_intr()
{
	if(wait_hd() > 0)
	{
		port_read(HD_PRI_DATA,hdbuffer,256);
	}
}

void write_intr()
{
}

void print_identify(char * info)
{
	#if 1
	HD_IDENTIFY_T * hd_info = (HD_IDENTIFY_T *)info;
	printf("HD cyl num: %d; head num: %d; sectors per track: %d\r\n", hd_info->wNumCyls, hd_info->wNumHeads, hd_info->wNumCurSectorsPerTrack);
	hd_info->sSerialNumber[19] = 0;
	hd_info->sModelNumber[39] = 0;
	printf("HD serial num: %s; mode num: %s;\r\n", hd_info->sSerialNumber,hd_info->sModelNumber);
	printf("HD support DMA: %s; support LBA: %s;\r\n", ((hd_info->wCapabilities.DMA) == 1)?"YES":"NO", ((hd_info->wCapabilities.LBA) == 1)?"YES":"NO");
	#endif
}

void identify_intr()
{
	if(wait_hd() > 0)
	{
		port_read(HD_PRI_DATA,hdbuffer,256);
	}
	print_identify(hdbuffer);
}


void hd_identify(int drive)
{
	hd_cmd_t cmd = {0};
	cmd.device = 0xE0;
	cmd.command = SATA_IDENTIFY;
	cmd.ptr_hd = identify_intr;
	#if 1
	if(!is_hd_ready())
	{
		wait_hd();
	}
	#endif
	hd_cmd_excu(&cmd);
}

void init_hd()
{
	u8 * hd_num = (u8*)(0xc0000475);
	printf("there is %d hds\r\n",*hd_num);
	set_intr_gate(0x2E,&hd_interrupt);      // 设置硬盘中断门向量 int 0x2E(46)。
	outb(0x21, inb(0x21)&0xfb);				// 复位接联的主8259A int2 的屏蔽位，允许从片
	outb_p(inb(0xA1)&0xbf, 0xA1);           // 复位硬盘的中断请求屏蔽位（在从片上），允许
                                            // 硬盘控制器发送中断请求信号。
	hdbuffer = (char *)malloc(512);
	memset(hdbuffer,0,512);
	hd_identify(0);
	printf("init hd success!\n");
}

