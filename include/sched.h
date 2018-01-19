/*
filename:	sched.h
author:		wei-coder
date:		2018-1
purpose:	进程调度相关的数据定义
*/
#if 1

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

extern struct task_struct *task[NR_TASKS];
extern struct task_struct *last_task_used_math;
extern struct task_struct *current;
extern long volatile jiffies;
extern long startup_time;
extern u32* pdt;

// 第一个任务结构的定义
#define INIT_TASK \
{\
	0,\
	15,\
	15,\
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
	NULL,\
	NULL,\
	NULL,\
	0,\
	{\
		0,\
		PAGE_SIZE + (long) (&init_task),\
		_SELECTOR_KER_DS,\
		0,\
		0,\
		0,\
		0,\
		(long) &pdt,\
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
		},\
}

#define FIRST_TASK	task[0]				// 任务0 比较特殊，所以特意给它单独定义一个符号。
#define LAST_TASK	task[NR_TASKS-1]	// 任务数组中的最后一项任务。

#define FIRST_TSS_ENTRY	GDT_INDEX_TSS


//需要更新ESP0才可以跳转，否则跳转失败
extern inline void switch_to(int n) 
{
	struct {long a,b;} __tmp; 
	asm volatile("cmpl %%ecx,%2\n\t" \
	        "je 1f\n\t" \
	        "movw %%dx,%1\n\t" \
	        "xchgl %%ecx,%2\n\t" \
	        "ljmp %0\n\t" \
	        "1:" \
	        ::"m" (*&__tmp.a),"m" (*&__tmp.b), "m"(current),\
	        "d" (_SELECTOR_TSS),"c" ((long) task[n])); \
}

void schedule (void);
#endif

