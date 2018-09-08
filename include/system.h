/*
filename:	system.h
author:		��ʱ������linux0.11��ʵ��
date:		2018-1
purpose:	�����˽����û�ģʽ�������û�������صĺ꼰����
*/

#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "pm.h"

//// �л����û�ģʽ���С�
// �ú�������iret ָ��ʵ�ִ��ں�ģʽ�л����û�ģʽ����ʼ����0����
// �����ջָ��esp ��eax �Ĵ����С�
// ���Ƚ���ջ��ѡ���(SS)��ջ��_SELECTOR_USER_DS
// Ȼ�󽫱���Ķ�ջָ��ֵ(esp)��ջ��
// ����־�Ĵ���(eflags)������ջ��
// ��Task0 �����ѡ���(cs)��ջ��_SELECTOR_USER_CS
// ��������1 ��ƫ�Ƶ�ַ(eip)��ջ��
// ִ���жϷ���ָ������ת��������1 ����
// ��ʱ��ʼִ������0��
// ��ʼ���μĴ���ָ�򱾾ֲ�������ݶΡ�_SELECTOR_USER_DS
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

#define sti() asm volatile ("sti");// ���ж�Ƕ����꺯����

#define cli()  asm volatile ("cli");//���ж�

#define nop() asm volatile ("nop"::);// �ղ�����

#define iret() asm volatile ("retd");// �жϷ��ء�

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
