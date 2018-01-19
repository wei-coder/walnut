/*
filename:	sched.c
author:		wei-coder
date:		2018-1
purpose:	�����˽��̵�����صĴ�����
*/

#if 1
#include "sched.h"
#include "console.h"


extern int timer_interrupt (void);	// ʱ���жϴ������
extern int system_call(void);	// ϵͳ�����жϴ������(kernel/system_call.s,80)��

/*ȫ����������Ķ���*/
extern desc_t gdt_entry[GDT_ENTRY_LEN] ;

static union task_union init_task = { INIT_TASK, };							// �����ʼ���������
long volatile jiffies;														// �ӿ�����ʼ����ĵδ���ʱ��ֵ��10ms/�δ𣩡�
long startup_time;														// ����ʱ�䡣��1970:0:0:0 ��ʼ��ʱ��������
struct task_struct *current = &(init_task.task);								// ��ǰ����ָ�루��ʼ��Ϊ��ʼ���񣩡�
struct task_struct *last_task_used_math = NULL;								// ʹ�ù�Э�����������ָ�롣
struct task_struct *task[NR_TASKS] = { &(init_task.task), };					// ��������ָ�����顣
long user_stack[PAGE_SIZE >> 2];											//����ϵͳ��ջָ�룬4K��ָ��ָ�����һ�
stack_t stack_start = {&user_stack[PAGE_SIZE >> 2], _SELECTOR_KER_DS};		//��������0��ϵͳ��ջ��ַ



// ��ʾ�����nr �Ľ��̺š�����״̬���ں˶�ջ�����ֽ�������Լ����
void show_task (int nr, task_struct *p)
{
	int i, j = 4096 - sizeof (task_struct);

	printf ("%d: pid=%d, state=%d, ", nr, p->pid, p->state);
	i = 0;
	while (i < j && !((char *) (p + 1))[i])	// ���ָ���������ݽṹ�Ժ����0 ���ֽ�����
		i++;
	printf ("%d (of %d) chars free in kernel stack\n\r", i, j);
}

// ��ʾ�������������š����̺š�����״̬���ں˶�ջ�����ֽ�������Լ����
void show_stat (void)
{
	int i;

	for (i = 0; i < NR_TASKS; i++)// NR_TASKS ��ϵͳ�����ɵ������̣�����������64 ������
		if (task[i])		// ������include/kernel/sched.h ��4 �С�
			show_task (i, task[i]);
}



/*schedule()�ǵ��Ⱥ����� */
void schedule (void)
{
	int i, next, c;
	struct task_struct **p;

  /* ���ȳ������Ҫ���� */
	while (1)
	{
		c = -1;
		next = 0;
		i = NR_TASKS;
		p = &task[NR_TASKS];
		// ��δ���Ҳ�Ǵ�������������һ������ʼѭ�������������������������ۡ��Ƚ�ÿ������
		// ״̬�����counter����������ʱ��ĵݼ��δ������ֵ����һ��ֵ������ʱ�仹������next ��
		// ָ���ĸ�������š�
		while (--i)
		{
			if (!*--p)
			{
				continue;
			}
			if ((*p)->state == TASK_RUNNING && (*p)->counter > c)
			{
				c = (*p)->counter;
				next = i;
			}
		}

		//����ҵ��˼�������0���������л����񼴿�
		if (c)
		{
			break;
		}
		// ����͸���ÿ�����������Ȩֵ������ÿһ�������counter ֵ��Ȼ�����±Ƚϡ�
		// counter ֵ�ļ��㷽ʽΪcounter = counter /2 + priority��[�ұ�counter=0]
		for (p = &LAST_TASK; p > &FIRST_TASK; --p)
		{
			if (*p)
			{
				(*p)->counter = ((*p)->counter >> 1) + (*p)->priority;
			}
		}
	}
	switch_to (next);		// �л��������Ϊnext �����񣬲�����֮��
}

// ���ȳ���ĳ�ʼ���ӳ���
void sched_init (void)
{
	gdt_set_gate(GDT_INDEX_TSS, TSS_FLAG, (u32)(&(init_task.task.tss)), sizeof(tss_struct));

	asm volatile("pushfl ; andl $0xffffbfff,(%esp) ; popfl");			// ��λNT ��־��

	asm volatile("ltr %%ax"::"a"(GDT_INDEX_TSS*8));				//����TR�Ĵ���

	//����ʱ���жϴ��������������ʱ���ж��ţ���
	//register_int_handler(IRQ0, &timer_interrupt);

	// ����ϵͳ�����ж��š�
	//set_system_gate (0x80, &system_call);
	idt_set_gate(0x80, (u32)(&system_call), _SELECTOR_KER_CS, 0x8E);
	
}

#endif
