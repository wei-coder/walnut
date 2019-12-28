/*
filename:	sched.h
author:		wei-coder
date:		2018-1
purpose:	进程调度相关的数据定义
*/
#ifndef __SCHED_H
#define __SCHED_H

#include <types.h>
#include "../mm/mm.h"
#include "../entry/pm.h"
#include "task.h"

// 定义系统时钟滴答频率(1000赫兹，每个滴答1ms)
#define HZ 1000

// 定义每个时间片的滴答数?。
#define LATCH (1193180/HZ)

//任务数上限，为调度简化，暂时设一个固定任务数
#define NR_TASKS	64

//任务联合体，因为任务控制块和进程堆栈方案一个内存页中。低地址为PCB，高低址为栈底。
union task_union
{
	struct task_struct task;	// 因为一个任务数据结构与其堆栈放在同一内存页中，所以
	char stack[PAGE_SIZE];		// 从堆栈段寄存器ss 可以获得其数据段选择符。
};

extern struct task_struct *task[NR_TASKS];
extern struct task_struct *last_task_used_math;
extern struct task_struct *current;
extern long volatile jiffies;
extern long startup_time;
extern pdt_t* pdt;

// 第一个任务结构的定义
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

#define FIRST_TASK	task[0]				// 任务0 比较特殊，所以特意给它单独定义一个符号。
#define LAST_TASK	task[NR_TASKS-1]	// 任务数组中的最后一项任务。

#define FIRST_TSS_ENTRY	GDT_INDEX_TSS

typedef int (*fn_ptr)();

//需要更新ESP0才可以跳转，否则跳转失败
//新任务的页表切换
//新任务的ESP需要在切换前刷新。
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

