/*
filename:	HD.c
author:	wei-code
date:	2018-04
prupose:	Ӳ�����������ʵ��
*/

#include "hd.h"
#include "io.h"
#include "pm.h"

extern void hd_interrupt();

void read_intr()
{

}

void write_intr()
{
}

void init_hd()
{
	set_intr_gate(0x2E,&hd_interrupt);      // ����Ӳ���ж������� int 0x2E(46)��
	outb(0x21, inb(0x21)&0xfb);          // ��λ��������8259A int2 ������λ�������Ƭ
	outb(0xA1, inb(0xA1)&0xbf);            // ��λӲ�̵��ж���������λ���ڴ�Ƭ�ϣ�������
                                            // Ӳ�̿����������ж������źš�
}
