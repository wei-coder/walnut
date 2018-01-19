/*
filename:	sched.h
author:		wei-coder
date:		2018-1
purpose:	���̵�����ص����ݶ���
*/
#if 1

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

extern struct task_struct *task[NR_TASKS];
extern struct task_struct *last_task_used_math;
extern struct task_struct *current;
extern long volatile jiffies;
extern long startup_time;
extern u32* pdt;

// ��һ������ṹ�Ķ���
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

#define FIRST_TASK	task[0]				// ����0 �Ƚ����⣬�������������������һ�����š�
#define LAST_TASK	task[NR_TASKS-1]	// ���������е����һ������

#define FIRST_TSS_ENTRY	GDT_INDEX_TSS


//��Ҫ����ESP0�ſ�����ת��������תʧ��
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

