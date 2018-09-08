/*
filename:	system.h
author:		暂时借用了linux0.11的实现
date:		2018-1
purpose:	定义了进入用户模式及设置用户进程相关的宏及数据
*/

#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "pm.h"

//// 切换到用户模式运行。
// 该函数利用iret 指令实现从内核模式切换到用户模式（初始任务0）。
// 保存堆栈指针esp 到eax 寄存器中。
// 首先将堆栈段选择符(SS)入栈。_SELECTOR_USER_DS
// 然后将保存的堆栈指针值(esp)入栈。
// 将标志寄存器(eflags)内容入栈。
// 将Task0 代码段选择符(cs)入栈。_SELECTOR_USER_CS
// 将下面标号1 的偏移地址(eip)入栈。
// 执行中断返回指令，则会跳转到下面标号1 处。
// 此时开始执行任务0，
// 初始化段寄存器指向本局部表的数据段。_SELECTOR_USER_DS
#define move_to_user_mode() \
asm volatile ("movl %%esp,%%eax\n\t" \
    "pushl $0x7B\n\t" \
    "pushl %%eax\n\t" \
    "pushfl\n\t" \
    "popl %%eax\n\t" \
    "or $0x3000,%%eax\n\t" \
    "pushl %%eax\n\t" \
    "pushl $0x73\n\t" \
    "pushl $1f\n\t" \
    "iret\n" \
    "1:\tmovl $0x7B,%%eax\n\t" \
    "movw %%ax,%%ds\n\t" \
    "movw %%ax,%%es\n\t" \
    "movw %%ax,%%fs\n\t" \
    "movw %%ax,%%gs" \
    :::"ax")

#define sti() asm volatile ("sti");// 开中断嵌入汇编宏函数。

#define cli()  asm volatile ("cli");//关中断

#define nop() asm volatile ("nop"::);// 空操作。

#define iret() asm volatile ("retd");// 中断返回。

#define _set_gate(gate_addr,type,dpl,addr) \
asm volatile ("movw %%dx,%%ax\n\t" \
    "movw %0,%%dx\n\t" \
    "movl %%eax,%1\n\t" \
    "movl %%edx,%2" \
    : \
    : "i" ((short) (0x8000+(dpl<<13)+(type<<8))), \
    "o" (*((char *) (gate_addr))), \
    "o" (*(4+(char *) (gate_addr))), \
    "d" ((char *) (addr)),"a" (0x00600000))

#define set_intr_gate(n,addr) \
    _set_gate(&idt_entry[n],14,0,addr)

#define set_trap_gate(n,addr) \
    _set_gate(&idt_entry[n],15,0,addr)

#define set_system_gate(n,addr) \
    _set_gate(&idt_entry[n],15,3,addr)


#endif
