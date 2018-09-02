/*
filename:	HD.c
author:	wei-code
date:	2018-04
prupose:	Ӳ�����������ʵ��
*/

#include "hd.h"
#include "io.h"
#include "pm.h"
#include "system.h"
#include "console.h"

extern void hd_interrupt();

void read_intr()
{

}

void write_intr()
{
}

void init_hd()
{
	u8 * hd_num = (u8*)(0x475);
	printf("there is %d hds\r\n",*hd_num);
	set_intr_gate(0x2E,&hd_interrupt);      // ����Ӳ���ж������� int 0x2E(46)��
	outb(0x21, inb(0x21)&0xfb);          // ��λ��������8259A int2 ������λ�������Ƭ
	outb_p(inb(0xA1)&0xbf, 0xA1);            // ��λӲ�̵��ж���������λ���ڴ�Ƭ�ϣ�������
                                            // Ӳ�̿����������ж������źš�
	printf("init hd success!\n");
}

void hd_cmd_excu(hd_cmd_t * cmd)
{
	//Ӳ���ж�ʹ��
	outb_p(cmd->ctl, HD_PRI_CONTROL);
	outb_p(cmd->feature, HD_PRI_ERR_FEAT);
	outb_p(cmd->count, HD_PRI_SEC_COUNT);
	outb_p(cmd->lba_low, HD_PRI_LBA_LOW);
	outb_p(cmd->lba_mid, HD_PRI_LBA_MID);
	outb_p(cmd->lba_high, HD_PRI_LBA_HIGH);
	outb_p(cmd->device, HD_PRI_DEVICE);
	outb_p(cmd->command, HD_PRI_STAT_COM);
}

bool is_hd_ready()
{
	u8 status = inb(HD_PRI_STAT_COM);
	if(status&0xC0 == 0x40)
	{
		return true;
	}
	return false;
}

void wait_hd()
{
	while(!is_hd_ready())
	{
		;
	}
}

void hd_identify(int drive)
{
	hd_cmd_t cmd = {0};
	cmd.device = SET_DEVICE_REG(0, drive, 0);
	cmd.command = SATA_IDENTIFY;
	if(!is_hd_ready())
	{
		wait_hd();
	}
	hd_cmd_excu(&cmd);
}
