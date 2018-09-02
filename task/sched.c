/*
filename:	sched.c
author:		wei-coder
date:		2018-1
purpose:	定义了进程调度相关的处理函数
*/

#if 1
#include "sched.h"
#include "console.h"
#include "pm.h"
#include "system.h"
#include "io.h"
#include "timer.h"
#include "logging.h"


extern int timer_interrupt (void);	// 时钟中断处理程序
extern int system_call(void);	// 系统调用中断处理程序(kernel/system_call.s,80)。

/*全局描述符表的定义*/
extern desc_t gdt_entry[GDT_ENTRY_LEN] ;

static union task_union init_task = { INIT_TASK, };							// 定义初始任务的数据
tss_struct tss = INIT_TSS;
long volatile jiffies;														// 从开机开始算起的滴答数时间值（10ms/滴答）。
long startup_time;														// 开机时间。从1970:0:0:0 开始计时的秒数。
struct task_struct *current = &(init_task.task);								// 当前任务指针（初始化为初始任务）。
struct task_struct *last_task_used_math = NULL;								// 使用过协处理器任务的指针。
struct task_struct *task[NR_TASKS] = { &(init_task.task), };					// 定义任务指针数组。


// 显示任务号nr 的进程号、进程状态和内核堆栈空闲字节数（大约）。
void show_task (int nr, task_struct *p)
{
	int i, j = 4096 - sizeof (task_struct);

	printf ("%d: pid=%d, state=%d, ", nr, p->pid, p->state);
	i = 0;
	while (i < j && !((char *) (p + 1))[i])	// 检测指定任务数据结构以后等于0 的字节数。
		i++;
	printf ("%d (of %d) chars free in kernel stack\n\r", i, j);
}

// 显示所有任务的任务号、进程号、进程状态和内核堆栈空闲字节数（大约）。
void show_stat (void)
{
	int i;

	for (i = 0; i < NR_TASKS; i++)// NR_TASKS 是系统能容纳的最大进程（任务）数量（64 个），
		if (task[i])		// 定义在include/kernel/sched.h 第4 行。
			show_task (i, task[i]);
}


/*schedule()是调度函数。 */
void schedule (void)
{
    int i,next,c;
    struct task_struct ** p;

    while (1) 
    {
        c = -1;
        next = 0;
        i = 65;
        p = &task[NR_TASKS];
        // 这段代码也是从任务数组的最后一个任务开始循环处理，并跳过不含任务的数组槽。比较每个就绪
        // 状态任务的counter（任务运行时间的递减滴答计数）值，哪一个值大，运行时间还不长，next 就
        // 指向哪个的任务号。
        while (--i) 
        {
            if (!*--p)
            {
                continue;
            }
            
            if ((*p)->state == TASK_RUNNING && (*p)->counter > c)
            {
                c = (*p)->counter, next = i-1;
            }
        }
        
        if (c) 
            break;
        // 否则就根据每个任务的优先权值，更新每一个任务的counter 值，然后回到125 行重新比较。
        // counter 值的计算方式为counter = counter /2 + priority。这里计算过程不考虑进程的状态。    
        for(p = &LAST_TASK ; p >= &FIRST_TASK ; --p)
        {
            if (*p)
            {
                (*p)->counter = ((*p)->counter >> 1) + (*p)->priority;
            }
        }
    }
    // 切换到任务号为next 的任务运行。因此若系统中没有任何其它任务
    // 可运行时，则next 始终为0。因此调度函数会在系统空闲时去执行任务0。此时任务0 仅执行
    // pause()系统调用，并又会调用本函数。
    switch_to(next);
}

#if 0
/*schedule()是调度函数。 */
void schedule (void)
{
	int i, next, c;
	struct task_struct **p;

  /* 调度程序的主要部分 */
	while (1)
	{
		c = -1;
		next = 0;
		i = NR_TASKS+1;
		p = &task[NR_TASKS];
		// 这段代码也是从任务数组的最后一个任务开始循环处理，并跳过不含任务的数组槽。比较每个就绪
		// 状态任务的counter（任务运行时间的递减滴答计数）值，哪一个值大，运行时间还不长，next 就
		// 指向哪个的任务号。
		while (--i)
		{
			if (!*--p)
			{
				continue;
			}
			if ((*p)->state == TASK_RUNNING && (*p)->counter > c)
			{
				c = (*p)->counter;
				next = i-1;
			}
		}

		printf("schedule next pid:%d; count:%d\n", task[next]->pid, c);
		//如果找到了计数大于0的任务，则切换任务即可
		if (c)
		{
			break;
		}
		// 否则就根据每个任务的优先权值，更新每一个任务的counter 值，然后重新比较。
		// counter 值的计算方式为counter = counter /2 + priority。[右边counter=0]
		for (p = &FIRST_TASK; p < &LAST_TASK; ++p)
		{
			if (*p)
			{
				(*p)->counter = ((*p)->counter >> 1) + (*p)->priority;
			}
		}
	}
	//需要更新ESP0才能切换，否则切换失败
	//task[next]->tss.esp0 = (long)&(task[next]) + PAGE_SIZE;
	switch_to (next);		// 切换到任务号为next 的任务，并运行之。
}
#endif

// 调度程序的初始化子程序。
void sched_init (void)
{
	init_task.task.pdt = (long)pdt-PAGE_OFFSET;
	init_task.task.esp0 = PAGE_SIZE+(long)&init_task;
	tss.cr3 = (long)pdt-PAGE_OFFSET;
	tss.esp0 = PAGE_SIZE+(long)&init_task;
	
	gdt_set_gate(GDT_INDEX_TSS, TSS_FLAG, (u32)(&(tss)), sizeof(tss_struct));

	asm volatile("pushfl ; andl $0xffffbfff,(%esp) ; popfl");			// 复位NT 标志。

	asm volatile("ltr %%ax"::"a"(_SELECTOR_TSS));				//加载TR寄存器

	//设置时钟中断处理程序句柄（设置时钟中断门）。
	set_intr_gate(0x20,&timer_interrupt);
	// 修改中断控制器屏蔽码，允许时钟中断。
	outb_p(inb_p(0x21)&~0x01,0x21);

	// 设置系统调用中断门。
	set_system_gate (0x80, &system_call);
	logging("init process schedule is success!\n");
}

#endif
