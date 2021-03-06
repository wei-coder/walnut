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
#define outb(port,value) \
__asm__ ("outb %%al,%%dx"::"a" (value),"d" (port))

/**************************************************************
函数说明: 向端口写两个字节
***************************************************************/
#define outw(port,value)\
	asm volatile("outw %%ax,%%dx"::"a" (value),"d" (port))

/**************************************************************
函数说明: 向端口写四个字节
***************************************************************/
#define outl(port,value)\
	asm volatile("outl %%eax,%%dx"::"a" (value),"d" (port))

/**************************************************************
函数说明: 向端口读一个字节
***************************************************************/
#define inb(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al":"=a" (_v):"d" (port)); \
_v; \
})

/**************************************************************
函数说明: 向端口读两个字节
***************************************************************/
#define inw(port) ({ \
unsigned short _v; \
__asm__ volatile ("inw %%dx,%%ax":"=a" (_v):"d" (port)); \
_v; \
})

/**************************************************************
函数说明: 向端口读四个字节
***************************************************************/
#define inl(port) ({ \
unsigned long _v; \
__asm__ volatile ("inl %%dx,%%eax":"=a" (_v):"d" (port)); \
_v; \
})

/**************************************************************
函数说明: 带延时的向端口写一个字节函数
***************************************************************/
#define outb_p(value,port) \
__asm__ ("outb %%al,%%dx\n" \
        "\tjmp 1f\n" \
        "1:\tjmp 1f\n" \
        "1:"::"a" (value),"d" (port))

/**************************************************************
函数说明: 带延时的向端口写两个字节函数
***************************************************************/
#define outw_p(value,port) \
__asm__ ("outw %%ax,%%dx\n" \
        "\tjmp 1f\n" \
        "1:\tjmp 1f\n" \
        "1:"::"a" (value),"d" (port))

/**************************************************************
函数说明: 带延时的向端口写一个字节函数
***************************************************************/
#define outl_p(value,port) \
__asm__ ("outl %%eax,%%dx\n" \
        "\tjmp 1f\n" \
        "1:\tjmp 1f\n" \
        "1:"::"a" (value),"d" (port))

/**************************************************************
函数说明: 带延时的向端口读一个字节函数
***************************************************************/
#define inb_p(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al\n" \
    "\tjmp 1f\n" \
    "1:\tjmp 1f\n" \
    "1:":"=a" (_v):"d" (port)); \
_v; \
})

/**************************************************************
函数说明: 带延时的向端口读两个字节函数
***************************************************************/
#define inw_p(port) ({ \
unsigned short _v; \
__asm__ volatile ("inw %%dx,%%ax\n" \
    "\tjmp 1f\n" \
    "1:\tjmp 1f\n" \
    "1:":"=a" (_v):"d" (port)); \
_v; \
})

/**************************************************************
函数说明: 带延时的向端口读四个字节函数
***************************************************************/
#define inl_p(port) ({ \
unsigned long _v; \
__asm__ volatile ("inl %%dx,%%eax\n" \
    "\tjmp 1f\n" \
    "1:\tjmp 1f\n" \
    "1:":"=a" (_v):"d" (port)); \
_v; \
})

#endif
