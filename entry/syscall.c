/*
filename:	syscall.c
author:		暂时借用了linux0.11的实现
date:		2018-1
purpose:	系统调用的相关函数实现
*/

#include "sched.h"

extern long volatile jiffies;							// 从开机开始算起的滴答数时间值（10ms/滴答）。
extern long startup_time;								// 开机时间。从1970:0:0:0 开始计时的秒数。
extern struct task_struct *current;						// 当前任务指针（初始化为初始任务）。
extern struct task_struct *task[NR_TASKS];				// 定义任务指针数组。
extern long user_stack[PAGE_SIZE >> 2];					//定义系统堆栈指针，4K。指针指在最后一项。

//stack_t stack_start = {&user_stack[PAGE_SIZE >> 2], _SELECTOR_KER_DS};		//定义任务0的系统堆栈地址


//// 时钟中断C 函数处理程序，在kernel/system_call.s 中的_timer_interrupt（176 行）被调用。
// 参数cpl 是当前特权级0 或3，0 表示内核代码在执行。
// 对于一个进程由于执行时间片用完时，则进行任务切换。并执行一个计时更新工作。
void do_timer (long cpl)
{
  // 如果当前特权级(cpl)为0（最高，表示是内核程序在工作），则将超级用户运行时间stime 递增；
  // 如果cpl > 0，则表示是一般用户程序在工作，增加utime。
	if (cpl)
		current->utime++;
	else
		current->stime++;

	if ((--current->counter) > 0)
		return;			// 如果进程运行时间还没完，则退出。
	current->counter = 0;
	if (!cpl)
		return;			// 对于超级用户程序，不依赖counter 值进行调度。
	schedule();
}

// 系统调用功能 - 设置报警定时时间值(秒)。
// 如果已经设置过alarm 值，则返回旧值，否则返回0。
int sys_alarm (long seconds)
{
	int old = current->alarm;

	if (old)
		old = (old - jiffies) / HZ;
	current->alarm = (seconds > 0) ? (jiffies + HZ * seconds) : 0;
	return (old);
}

// 取当前进程号pid。
int sys_getpid (void)
{
	return current->pid;
}

// 取父进程号ppid。
int sys_getppid (void)
{
	return current->father;
}

// 取用户号uid。
int sys_getuid (void)
{
	return current->uid;
}

// 取euid。
int sys_geteuid (void)
{
	return current->euid;
}

// 取组号gid。
int sys_getgid (void)
{
	return current->gid;
}

// 取egid。
int sys_getegid (void)
{
	return current->egid;
}

// 系统调用功能 -- 降低对CPU 的使用优先权（有人会用吗？?）。
// 应该限制increment 大于0，否则的话,可使优先权增大！！
int sys_nice (long increment)
{
	if (current->priority - increment > 0)
		current->priority -= increment;
	return 0;
}

