/*
filename:	io.h
author:	wei-coder
date:	2017-12
purpose:	IO操作的头文件，内联函数定义
*/

#ifndef __IO_H
#define __IO_H

#include "types.h"

/**************************************************************
函数说明: 向端口写一个字节
***************************************************************/
static inline void outb(u16 port, u8 value)
{
	asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
};

/**************************************************************
函数说明: 向端口读一个字节
***************************************************************/
static inline u8 inb(u16 port)
{
	u8 ret;
	asm volatile("inb %1, %0": "=a"(ret):"dN"(port));
	return ret;
};

/**************************************************************
函数说明: 向端口写两个字节
***************************************************************/
static inline void outw(u16 port)
{
	u16 ret;
	asm volatile("inw %1, %0": "=a"(ret):"dN"(port));
};


#define outb_p(value,port) \
__asm__ ("outb %%al,%%dx\n" \
        "\tjmp 1f\n" \
        "1:\tjmp 1f\n" \
        "1:"::"a" (value),"d" (port))

#define inb_p(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al\n" \
    "\tjmp 1f\n" \
    "1:\tjmp 1f\n" \
    "1:":"=a" (_v):"d" (port)); \
_v; \
})


#endif
