/*
filename:	timer.c
author:		wei-coder
date:		2017-12
purpose:	ʱ���ж���غ�������
*/

#include "timer.h"
#include "io.h"
#include "pm.h"
#include "console.h"


void timer_callback(int_cont_t *context)
{
	return;
}

void init_timer(u32 frequency)
{
	register_int_handler(IRQ0, timer_callback);

	// Intel 8253/8254 оƬPIT I/�˿ڵ�ַ��Χ��O40h~43h
	// ����Ƶ��Ϊ�� 1193180frequency ��ÿ���жϴ���
	u32 divisor = 1193180 / frequency;

	// D7 D6 D5 D4 D3 D2 D1 D0
	// 0    0    1   1   0   1   1   0
	// ����36 H
	// ����8253/8254 оƬ������ģʽ3 ��
	outb(0x43, 0x36);

	// ��ֵ��ֽں͸��ֽ�
	u8 low = (u8)(divisor & 0xFF);
	u8 hign = (u8)((divisor >> 8) & 0xFF);

	// �ֱ�д����ֽں͸��ֽ�
	outb(0x40, low);
	outb(0x40, hign);
}

