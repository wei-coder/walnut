/*
filename:	task.h
author:		wei-coder
date:		2018-1
purpose:	线程、进程相关的数据结构定义
*/

#ifndef __TASK_H
#define __TASK_H

#include "types.h"
#include "signal.h"

/*进程状态*/
#define	TASK_UNNABLE	-1		//任务不可运行
#define TASK_RUNNING	0		// 进程正在运行或已准备就绪。
#define TASK_INTERRUPTIBLE 1	// 进程处于可中断等待状态。
#define TASK_UNINTERRUPTIBLE 2	// 进程处于不可中断等待状态，主要用于I/O 操作等待。
#define TASK_ZOMBIE 3			// 进程处于僵死状态，已经停止运行，但父进程还没发信号。
#define TASK_STOPPED 4			// 进程已停止。

/*内核线程的上下文切换保存的信息*/
struct context
{
	u32	esp;		//堆栈指针
	u32	ebp;		//栈低指针
	u32	ebx;		//基址寄存器
	u32	esi;		//源索引
	u32	edi;		//目标索引
	u32	eflags;		//状态寄存器
};

// 任务状态段数据结构
typedef struct tss_struct
{
	long back_link;		/* 16 high bits zero */
	long esp0;
	long ss0;				/* 16 high bits zero */
	long esp1;
	long ss1;				/* 16 high bits zero */
	long esp2;
	long ss2;				/* 16 high bits zero */
	long cr3;
	long eip;
	long eflags;
	long eax;
	long ecx;
	long edx;
	long ebx;
	long esp;
	long ebp;
	long esi;
	long edi;
	long es;			/* 16 high bits zero */
	long cs;			/* 16 high bits zero */
	long ss;			/* 16 high bits zero */
	long ds;			/* 16 high bits zero */
	long fs;			/* 16 high bits zero */
	long gs;			/* 16 high bits zero */
	long ldt;
	long io_bitmap;
}tss_struct;


/*进程控制块*/
typedef struct task_struct
{
	long state;						// long state 任务的运行状态（-1 不可运行，0 可运行(就绪)，>0 已停止）。
	long counter;					// long counter 任务运行时间计数(递减)（滴答数），运行时间片。
	long priority;					// 运行优先数。任务开始运行时counter = priority，越大运行越长。
	long signal;					//信号。是位图，每个比特位代表一种信号，信号值=位偏移值+1。
	struct sigaction sigaction[32];	//信号执行属性结构，对应信号将要执行的操作和标志信息。
	long blocked;					//进程信号屏蔽码（对应信号位图）。
	
	int exit_code;					//任务执行停止的退出码，其父进程会取。
	ulong start_code;				//代码段地址。
	ulong end_code;					//代码长度（字节数）。
	ulong end_data;					//代码长度 + 数据长度（字节数）。
	ulong brk;						//总长度（字节数）。
	ulong start_stack;				//堆栈段地址。
	long pid;						//进程标识号(进程号)。
	long father;					//父进程号。
	long pgrp;						//父进程组号。
	long session;					//会话号。
	long leader;					//会话首领。
	u16	uid;						//用户标识号（用户id）。
	u16	euid;						//有效用户id。
	u16	suid;						//保存的用户id。
	u16 gid;						//组标识号（组id）。
	u16	egid;						//有效组id。
	u16 sgid;						//保存的组id。
	long alarm;						//报警定时值（滴答数）。
	long utime;						//用户态运行时间（滴答数）。
	long stime;						//系统态运行时间（滴答数）。
	long cutime;					//子进程用户态运行时间。
	long cstime;					//子进程系统态运行时间。
	long start_time;				//进程开始运行时刻。
	u16 used_math;					//标志：是否使用了协处理器。
	
/* file system info */
	int tty;						//进程使用tty 的子设备号。-1 表示没有使用。
	u16 umask;						//文件创建属性屏蔽位。
	u32 *pwd;						//当前工作目录i 节点结构。 暂时通过普通指针代替
	u32 *root;						//根目录i 节点结构。暂时通过普通指针代替
	u32 *executable;				//执行文件i 节点结构。暂时通过普通指针代替
	ulong close_on_exec;			//执行时关闭文件句柄位图标志。（参见include/fcntl.h）
	u32	pdt;
	long esp;
	long esp0;
	long eip;

	//struct desc_t ldt[3];			//本任务的局部表描述符。0-空，1-代码段cs，2-数据和堆栈段ds&ss。
	//tss_struct tss;				//本进程的任务状态段信息结构。
}task_struct;

#endif
