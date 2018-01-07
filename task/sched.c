/*
filename:	sched.c
author:		wei-coder
date:		2018-1
purpose:	�����˽��̵�����صĴ�����
*/

#include "sched.h"
#include "pm.h"
#include "system.h"

extern int timer_interrupt (void);	// ʱ���жϴ������
extern int system_call(void);	// ϵͳ�����жϴ������(kernel/system_call.s,80)��

/*ȫ����������Ķ���*/
extern desc_t gdt_entry[GDT_ENTRY_LEN] = {0};

static union task_union init_task = { INIT_TASK, };							// �����ʼ���������
long volatile jiffies;														// �ӿ�����ʼ����ĵδ���ʱ��ֵ��10ms/�δ𣩡�
long startup_time;															// ����ʱ�䡣��1970:0:0:0 ��ʼ��ʱ��������
struct task_struct *current = &(init_task.task);							// ��ǰ����ָ�루��ʼ��Ϊ��ʼ���񣩡�
struct task_struct *last_task_used_math = NULL;								// ʹ�ù�Э�����������ָ�롣
struct task_struct *task[NR_TASKS] = { &(init_task.task), };				// ��������ָ�����顣
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
	int i;
	desc_t *p;
	u8 n = _SELECTOR_TSS;

	set_tss_desc((u16*)&gdt_entry[FIRST_TSS_ENTRY], &(init_task.task.tss));
	//set_ldt_desc (gdt + FIRST_LDT_ENTRY, &(init_task.task.ldt));
	// ��������������������ע��i=1 ��ʼ�����Գ�ʼ��������������ڣ���
	p = &gdt_entry[FIRST_TSS_ENTRY];

	_asm pushfd;
	_asm and dword ptr ss:[esp],0xffffbfff;
	_asm popfd;
	_asm{ltr word ptr n}			// ������0 ��TSS ���ص�����Ĵ���tr��

	//����ʱ���жϴ��������������ʱ���ж��ţ���
	register_int_handler(IRQ0, &timer_interrupt);

  // ����ϵͳ�����ж��š�
	set_system_gate (0x80, &system_call);
}

