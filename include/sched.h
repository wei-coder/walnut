/*
filename:	sched.h
author:		wei-coder
date:		2018-1
purpose:	进程调度相关的数据定义
*/

#include "task.h"
#include "memory.h"
#include "pm.h"

// 定义系统时钟滴答频率(1 百赫兹，每个滴答10ms)
#define HZ 100

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

// 该结构用于设置堆栈ss:esp（数据段选择符，指针），见head.s，第23 行。
typedef struct STACK_T
{
  long *a;
  short b;
}stack_t;

// 第一个任务结构的定义
#define INIT_TASK \
{\
	/*long state;*/ 0,\
	/*long counter;*/ 15,\
	/*long priority;*/ 15,\
	/*long signal;*/ 0,\
	/*long blocked;*/ 0,\
	
	/*int exit_code;*/	0,\
	/*ulong start_code;*/ 0,\
	/*ulong end_code;*/	 0,\
	/*ulong end_data;*/	 0,\
	/*ulong brk;*/		 0,\
	/*ulong start_stack;*/ 0,\
	/*long pid;*/		 0,\
	/*long father;*/	-1,\
	/*long pgrp;*/		 0,\
	/*long session;*/	 0,\
	/*long leader;*/	 0,\
	/*u16	uid;*/		 0,\
	/*u16	euid;*/		 0,\
	/*u16	suid;*/		 0,\
	/*u16 gid;*/		 0,\
	/*u16	egid;*/		 0,\
	/*u16 sgid;*/		 0,\
	/*long alarm;*/		 0,\
	/*long utime;*/		 0,\
	/*long stime;*/		 0,\
	/*long cutime;*/	 0,\
	/*long cstime;*/	 0,\
	/*long start_time;*/ 0,\
	/*u16 used_math;*/	 0,\

/* file system info */
	/*int tty;*/		-1,\
	/*u16 umask;*/		0022,\
	/*u32 *pwd;*/		NULL,\
	/*u32 *root;*/		NULL,\
	/*u32 *executable;*/	NULL,\
	/*ulong close_on_exec;*/ 0,\
	/*struct context tss;*/ {\
					/*long back_link;*/		0,\
					/*long esp0;*/			PAGE_SIZE + (long) (&init_task),\
					/*long ss0;*/			_SELECTOR_KER_DS,\
					/*long esp1;*/			0,\
					/*long ss1;*/			0,\
					/*long esp2;*/			0,\
					/*long ss2;*/			0,\
					/*long cr3;*/			(long) &pdt,\
    				/*long eip;*/			0,\
    				/*long eflags;*/		0,\
    				/*long eax;*/			0,\
    				/*long ecx;*/			0,\
    				/*long edx;*/			0,\
    				/*long ebx;*/			0,\
    				/*long esp;*/			0,\
    				/*long ebp;*/			0,\
    				/*long esi;*/			0,\
    				/*long edi;*/			0,\
    				/*long es;*/			_SELECTOR_USER_DS,\
    				/*long cs;*/			_SELECTOR_USER_CS,\
    				/*long ss;*/			_SELECTOR_USER_DS,\
    				/*long ds;*/			_SELECTOR_USER_DS,\
    				/*long fs;*/			_SELECTOR_USER_DS,\
    				/*long gs;*/			_SELECTOR_USER_DS,},\
}

#define FIRST_TASK	task[0]				// 任务0 比较特殊，所以特意给它单独定义一个符号。
#define LAST_TASK	task[NR_TASKS-1]	// 任务数组中的最后一项任务。

#define FIRST_TSS_ENTRY	GDT_INDEX_TSS

extern inline void switch_to(int n) 
{
		unsigned short __tmp;
		__tmp = _SELECTOR_TSS;

		asm volatile(\
			"mov ebx, %0;"
			"mov eax, %1;"
			"mov ecx, %2"
			"cmp ecx, %3"
			"je l1"
			"xchg ecx,%3"
			"mov ax, %4"
			"mov word ptr ds:[lcs],ax"
			:"=r"(offset task), "=m"(n),"=m"([ebx+eax*4]), "=m"(current), "=m"(__tmp) );
		asm volatile(
			"_emit 0xea"
			"_emit 0"
			"_emit 0"
			"_emit 0" 
			"_emit 0");
	lcs:
		asm volatile(
			"_emit 0"
			"_emit 0");
	l1:;
}

void schedule (void);

