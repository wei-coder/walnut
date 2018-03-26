/*
filename: init.c
author:	  wei-coder
date:	  2018-1
purpose:  定义了用户态的第一个进程
*/

#include "init.h"
#include "console.h"
#include "sched.h"

//extern struct task_struct *current;


void init(void)
{
	printf("current process is %d!\n", current->pid);
}

