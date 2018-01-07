/*
filename:	sched.h
author:		wei-coder
date:		2018-1
purpose:	���̵�����ص����ݶ���
*/

#include "task.h"
#include "memory.h"
#include "pm.h"

// ����ϵͳʱ�ӵδ�Ƶ��(1 �ٺ��ȣ�ÿ���δ�10ms)
#define HZ 100

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

// �ýṹ�������ö�ջss:esp�����ݶ�ѡ�����ָ�룩����head.s����23 �С�
typedef struct STACK_T
{
  long *a;
  short b;
}stack_t;

// ��һ������ṹ�Ķ���
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

#define FIRST_TASK	task[0]				// ����0 �Ƚ����⣬�������������������һ�����š�
#define LAST_TASK	task[NR_TASKS-1]	// ���������е����һ������

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

