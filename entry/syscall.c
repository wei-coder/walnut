/*
filename:	syscall.c
author:		��ʱ������linux0.11��ʵ��
date:		2018-1
purpose:	ϵͳ���õ���غ���ʵ��
*/

#include "sched.h"

extern long volatile jiffies;							// �ӿ�����ʼ����ĵδ���ʱ��ֵ��10ms/�δ𣩡�
extern long startup_time;								// ����ʱ�䡣��1970:0:0:0 ��ʼ��ʱ��������
extern struct task_struct *current;						// ��ǰ����ָ�루��ʼ��Ϊ��ʼ���񣩡�
extern struct task_struct *task[NR_TASKS];				// ��������ָ�����顣
extern long user_stack[PAGE_SIZE >> 2];					//����ϵͳ��ջָ�룬4K��ָ��ָ�����һ�

//stack_t stack_start = {&user_stack[PAGE_SIZE >> 2], _SELECTOR_KER_DS};		//��������0��ϵͳ��ջ��ַ


//// ʱ���ж�C �������������kernel/system_call.s �е�_timer_interrupt��176 �У������á�
// ����cpl �ǵ�ǰ��Ȩ��0 ��3��0 ��ʾ�ں˴�����ִ�С�
// ����һ����������ִ��ʱ��Ƭ����ʱ������������л�����ִ��һ����ʱ���¹�����
void do_timer (long cpl)
{
  // �����ǰ��Ȩ��(cpl)Ϊ0����ߣ���ʾ���ں˳����ڹ��������򽫳����û�����ʱ��stime ������
  // ���cpl > 0�����ʾ��һ���û������ڹ���������utime��
	if (cpl)
		current->utime++;
	else
		current->stime++;

	if ((--current->counter) > 0)
		return;			// �����������ʱ�仹û�꣬���˳���
	current->counter = 0;
	if (!cpl)
		return;			// ���ڳ����û����򣬲�����counter ֵ���е��ȡ�
	schedule();
}

// ϵͳ���ù��� - ���ñ�����ʱʱ��ֵ(��)��
// ����Ѿ����ù�alarm ֵ���򷵻ؾ�ֵ�����򷵻�0��
int sys_alarm (long seconds)
{
	int old = current->alarm;

	if (old)
		old = (old - jiffies) / HZ;
	current->alarm = (seconds > 0) ? (jiffies + HZ * seconds) : 0;
	return (old);
}

// ȡ��ǰ���̺�pid��
int sys_getpid (void)
{
	return current->pid;
}

// ȡ�����̺�ppid��
int sys_getppid (void)
{
	return current->father;
}

// ȡ�û���uid��
int sys_getuid (void)
{
	return current->uid;
}

// ȡeuid��
int sys_geteuid (void)
{
	return current->euid;
}

// ȡ���gid��
int sys_getgid (void)
{
	return current->gid;
}

// ȡegid��
int sys_getegid (void)
{
	return current->egid;
}

// ϵͳ���ù��� -- ���Ͷ�CPU ��ʹ������Ȩ�����˻�����?����
// Ӧ������increment ����0������Ļ�,��ʹ����Ȩ���󣡣�
int sys_nice (long increment)
{
	if (current->priority - increment > 0)
		current->priority -= increment;
	return 0;
}

