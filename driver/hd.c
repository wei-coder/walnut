<<<<<<< HEAD
/*
filename: hd.c
author:	  wei-coder
date:	  2018-6
purpose:  硬盘驱动
*/

int read_block()
{

}

int write_block()
{

}
=======
/*
filename:	HD.c
author:	wei-code
date:	2018-04
prupose:	硬盘驱动的相关实现
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
	set_intr_gate(0x2E,&hd_interrupt);      // 设置硬盘中断门向量 int 0x2E(46)。
	outb(0x21, inb(0x21)&0xfb);          // 复位接联的主8259A int2 的屏蔽位，允许从片
	outb(0xA1, inb(0xA1)&0xbf);            // 复位硬盘的中断请求屏蔽位（在从片上），允许
                                            // 硬盘控制器发送中断请求信号。
}
>>>>>>> 2747b789ab05c6901e2e08b9db7fbb72fd8f6f24
