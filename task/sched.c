/*
filename:	sched.c
author:		wei-coder
date:		2018-1
purpose:	�����˽��̵�����صĴ�����
*/

#if 1
#include "sched.h"
#include "console.h"
#include "pm.h"
#include "system.h"
#include "io.h"
#include "timer.h"
#include "logging.h"


extern int timer_interrupt (void);	// ʱ���жϴ������
extern int system_call(void);	// ϵͳ�����жϴ������(kernel/system_call.s,80)��

/*ȫ����������Ķ���*/
extern desc_t gdt_entry[GDT_ENTRY_LEN] ;

static union task_union init_task = { INIT_TASK, };							// �����ʼ���������
tss_struct tss = INIT_TSS;
long volatile jiffies;														// �ӿ�����ʼ����ĵδ���ʱ��ֵ��10ms/�δ𣩡�
long startup_time;														// ����ʱ�䡣��1970:0:0:0 ��ʼ��ʱ��������
struct task_struct *current = &(init_task.task);								// ��ǰ����ָ�루��ʼ��Ϊ��ʼ���񣩡�
struct task_struct *last_task_used_math = NULL;								// ʹ�ù�Э�����������ָ�롣
struct task_struct *task[NR_TASKS] = { &(init_task.task), };					// ��������ָ�����顣


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
    int i,next,c;
    struct task_struct ** p;

    while (1) 
    {
        c = -1;
        next = 0;
        i = 65;
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
                c = (*p)->counter, next = i-1;
            }
        }
        
        if (c) 
            break;
        // ����͸���ÿ�����������Ȩֵ������ÿһ�������counter ֵ��Ȼ��ص�125 �����±Ƚϡ�
        // counter ֵ�ļ��㷽ʽΪcounter = counter /2 + priority�����������̲����ǽ��̵�״̬��    
        for(p = &LAST_TASK ; p >= &FIRST_TASK ; --p)
        {
            if (*p)
            {
                (*p)->counter = ((*p)->counter >> 1) + (*p)->priority;
            }
        }
    }
    // �л��������Ϊnext ���������С������ϵͳ��û���κ���������
    // ������ʱ����next ʼ��Ϊ0����˵��Ⱥ�������ϵͳ����ʱȥִ������0����ʱ����0 ��ִ��
    // pause()ϵͳ���ã����ֻ���ñ�������
    switch_to(next);
}

#if 0
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
		i = NR_TASKS+1;
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
				next = i-1;
			}
		}

		printf("schedule next pid:%d; count:%d\n", task[next]->pid, c);
		//����ҵ��˼�������0���������л����񼴿�
		if (c)
		{
			break;
		}
		// ����͸���ÿ�����������Ȩֵ������ÿһ�������counter ֵ��Ȼ�����±Ƚϡ�
		// counter ֵ�ļ��㷽ʽΪcounter = counter /2 + priority��[�ұ�counter=0]
		for (p = &FIRST_TASK; p < &LAST_TASK; ++p)
		{
			if (*p)
			{
				(*p)->counter = ((*p)->counter >> 1) + (*p)->priority;
			}
		}
	}
	//��Ҫ����ESP0�����л��������л�ʧ��
	//task[next]->tss.esp0 = (long)&(task[next]) + PAGE_SIZE;
	switch_to (next);		// �л��������Ϊnext �����񣬲�����֮��
}
#endif

// ���ȳ���ĳ�ʼ���ӳ���
void sched_init (void)
{
	init_task.task.pdt = (long)pdt-PAGE_OFFSET;
	init_task.task.esp0 = PAGE_SIZE+(long)&init_task;
	tss.cr3 = (long)pdt-PAGE_OFFSET;
	tss.esp0 = PAGE_SIZE+(long)&init_task;
	
	gdt_set_gate(GDT_INDEX_TSS, TSS_FLAG, (u32)(&(tss)), sizeof(tss_struct));

	asm volatile("pushfl ; andl $0xffffbfff,(%esp) ; popfl");			// ��λNT ��־��

	asm volatile("ltr %%ax"::"a"(_SELECTOR_TSS));				//����TR�Ĵ���

	//����ʱ���жϴ��������������ʱ���ж��ţ���
	set_intr_gate(0x20,&timer_interrupt);
	// �޸��жϿ����������룬����ʱ���жϡ�
	outb_p(inb_p(0x21)&~0x01,0x21);

	// ����ϵͳ�����ж��š�
	set_system_gate (0x80, &system_call);
	logging("init process schedule is success!\n");
}

#endif
