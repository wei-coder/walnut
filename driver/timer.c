/*
filename:	timer.c
author:		wei-coder
date:		2017-12
purpose:	时钟中断相关函数定义
*/

#include <system.h>
#include <kio.h>
#include "../util/logging.h"
#include "../entry/pm.h"
#include "../task/sched.h"
#include "timer.h"

void timer_callback()
{
	printf("this is process %d is running!\n", current->pid);
}

void init_timer(u32 frequency)
{
	// Intel 8253/8254 芯片PIT I/端口地址范围是O40h~43h
	// 输入频率为， 1193180frequency 即每秒中断次数
	u32 divisor = 1193180 / frequency;

	// D7 D6 D5 D4 D3 D2 D1 D0
	// 0    0    1   1   0   1   1   0
	// 就是36 H
	// 设置8253/8254 芯片工作在模式3 下
	outb(0x43, 0x36);

	// 拆分低字节和高字节
	u8 low = (u8)(divisor & 0xFF);
	u8 hign = (u8)((divisor >> 8) & 0xFF);

	// 分别写入低字节和高字节
	outb(0x40, low);
	outb(0x40, hign);
	logging("init timer is success!\n");
}

