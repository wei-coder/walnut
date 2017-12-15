/*
filename:	io.h
author:	wei-coder
date:	2017-12
purpose:	IO操作的头文件，内联函数定义
*/

#include "types.h"

/**************************************************************
函数说明: 向端口写一个字节
***************************************************************/
static inline void outb(u16 port, u8 value)
{
	asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
};

static inline u8 inb(u16 port)
{
	u8 ret;
	asm volatile("inb %1, %0": "=a"(ret):"dN"(port));
	return ret;
};

static inline void outw(u16 port)
{
	u16 ret;
	asm volatile("inw %1, %0": "=a"(ret):"dN"(port));
	return ret;
};
