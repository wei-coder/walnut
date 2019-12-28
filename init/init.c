/*
filename: init.c
author:	  wei-coder
date:	  2018-1
purpose:  定义了用户态的第一个进程
*/

#include <kio.h>
#include "../task/sched.h"
#include "init.h"

void init(void)
{
	printf("current process is %d!\n", current->pid);
}

