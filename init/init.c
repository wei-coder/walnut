/*
filename: init.c
author:	  wei-coder
date:	  2018-1
purpose:  �������û�̬�ĵ�һ������
*/

#include <kio.h>
#include "../task/sched.h"
#include "init.h"

void init(void)
{
	while(1){
		printf("this is process init_%d!\n", current->pid);
	}
}

