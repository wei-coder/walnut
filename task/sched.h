/*
filename:	sched.h
author:		wei-coder
date:		2018-1
purpose:	���̵�����ص����ݶ���
*/
#ifndef __SCHED_H
#define __SCHED_H

#include <types.h>
#include "../mm/mm.h"
#include "../entry/pm.h"
#include "task.h"

// ����ϵͳʱ�ӵδ�Ƶ��(1000���ȣ�ÿ���δ�1ms)
#define HZ 1000

// ����ÿ��ʱ��Ƭ�ĵδ���?��
#define LATCH (1193180/HZ)

//���������ޣ�Ϊ���ȼ򻯣���ʱ��һ���̶�������
#define NR_TASKS	64

//���������壬��Ϊ������ƿ�ͽ��̶�ջ����һ���ڴ�ҳ�С��͵�ַΪPCB���ߵ�ַΪջ�ס�
union task_union
{
	struct task_struct task;	// ��Ϊһ���������ݽṹ�����ջ����ͬһ�ڴ�ҳ�У�����
	char stack[PAGE_SIZE];		// �Ӷ�ջ�μĴ���ss ���Ի�������ݶ�ѡ�����
};

extern struct task_struct *task[NR_TASKS];
extern struct task_struct *last_task_used_math;
extern struct task_struct *current;
extern long volatile jiffies;
extern long startup_time;
extern pdt_t* pdt;

// ��һ������ṹ�Ķ���
#define INIT_TASK \
{\
	0,\
	15,\
	15,\
	0,\
	{{},},\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	-1,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	-1,\
	0022,\
	{0},\
	{0},\
	{0},\
	{0},\
	0,\
	0,\
	PAGE_SIZE+(long)&init_task,\
	0,\
	0\
}


#define INIT_TSS \
{\
	0,\
	PAGE_SIZE+(long)&init_task,\
	_SELECTOR_KER_DS,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	_SELECTOR_USER_DS,\
	_SELECTOR_USER_CS,\
	_SELECTOR_USER_DS,\
	_SELECTOR_USER_DS,\
	_SELECTOR_USER_DS,\
	_SELECTOR_USER_DS,\
	_SELECTOR_LDT,\
	0x80000000,\
}

#define FIRST_TASK	task[0]				// ����0 �Ƚ����⣬�������������������һ�����š�
#define LAST_TASK	task[NR_TASKS-1]	// ���������е����һ������

#define FIRST_TSS_ENTRY	GDT_INDEX_TSS

typedef int (*fn_ptr)();

//��Ҫ����ESP0�ſ�����ת��������תʧ��
//�������ҳ���л�
//�������ESP��Ҫ���л�ǰˢ�¡�
#define switch_to(n) \
{\
	asm volatile("cmpl %%ecx, current\n\t" \
		"je 1f\n\t" \
		"xchgl %%ecx, current\n\t" \
		"movl %%esp, %1\n\t"\
		"movl %%eax, %0\n\t"\
		"movl %%ebx, %%esp\n\t"\
		"movl %2, %%edx\n\t"\
		"movl %%edx,%%cr3\n\t" \
		"1:" \
		::"m"(tss.esp0),"m"(current->esp),"m"(task[n]->pdt),"a"(task[n]->esp0), "b"(task[n]->esp),"c" ((long) task[n])); \
}

void schedule (void);
void sched_init(void);

#endif

