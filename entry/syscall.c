/*
filename:	syscall.c
author:		��ʱ������linux0.11��ʵ��
date:		2018-1
purpose:	ϵͳ���õ���غ���ʵ��
*/

#include "syscall.h"

extern long volatile jiffies;							// �ӿ�����ʼ����ĵδ���ʱ��ֵ��10ms/�δ𣩡�
extern long startup_time;								// ����ʱ�䡣��1970:0:0:0 ��ʼ��ʱ��������
extern struct task_struct *current;						// ��ǰ����ָ�루��ʼ��Ϊ��ʼ���񣩡�
extern struct task_struct *task[NR_TASKS];				// ��������ָ�����顣
extern long user_stack[PAGE_SIZE >> 2];					//����ϵͳ��ջָ�룬4K��ָ��ָ�����һ�

ulong do_hd;
ulong do_floppy;

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


int sys_setup()
{
	return 0;
};

int sys_exit(int error_code)
{
	return -1;
}

int sys_read()
{
	return 0;
};
int sys_write()
{
	return 0;
};
int sys_open()
{
	return 0;
}
int sys_close()
{
	return 0;
}
	
int sys_waitpid()
{
	return 0;
}

int sys_creat()
{
	return 0;
}


int sys_link()
{
	return 0;
}


int sys_unlink()
{
	return 0;
}

int sys_chdir()
{
	return 0;
}

int sys_time()
{
	return 0;
}

int sys_mknod()
{
	return 0;
}

int sys_chmod()
{
	return 0;
}

int sys_chown()
{
	return 0;
}

int sys_break()
{
	return 0;
}

int sys_stat()
{
	return 0;
}

int sys_lseek()
{
	return 0;
}

int sys_mount()
{
	return 0;
}

int sys_umount()
{
	return 0;
}

int sys_setuid()
{
	return 0;
}

int sys_stime()
{
	return 0;
}

int sys_ptrace()
{
	return 0;
}

int sys_fstat()
{
	return 0;
}

int sys_pause()
{
	return 0;
}

int sys_utime()
{
	return 0;
}

int sys_stty()
{
	return 0;
}
int sys_gtty()
{
	return 0;
}

int sys_access()
{
	return 0;
}

int sys_ftime()
{
	return 0;
}
int sys_sync()
{
	return 0;
}

int sys_kill()
{
	return 0;
}

int sys_rename()
{
	return 0;
}

int sys_mkdir()
{
	return 0;
}

int sys_rmdir()
{
	return 0;
}

int sys_dup()
{
	return 0;
}

int sys_pipe()
{
	return 0;
}

int sys_times()
{
	return 0;
}

int sys_prof()
{
	return 0;
}

int sys_brk()
{
	return 0;
}

int sys_signal()
{
	return 0;
}

int sys_acct()
{
	return 0;
}

int sys_phys()
{
	return 0;
}

int sys_lock()
{
	return 0;
}

int sys_ioctl()
{
	return 0;
}

int sys_fcntl()
{
	return 0;
}

int sys_mpx()
{
	return 0;
}

int sys_ulimit()
{
	return 0;
}

int sys_uname()
{
	return 0;
}

int sys_umask()
{
	return 0;
}

int sys_chroot()
{
	return 0;
}

int sys_ustat()
{
	return 0;
}

int sys_dup2()
{
	return 0;
}

int sys_getpgrp()
{
	return 0;
}

int sys_setsid()
{
	return 0;
}

int sys_sigaction()
{
	return 0;
}

int sys_sgetmask()
{
	return 0;
}

int sys_ssetmask()
{
	return 0;
}

int sys_setreuid()
{
	return 0;
}

int sys_setregid()
{
	return 0;
}

void do_signal(long signr,long eax, long ebx, long ecx, long edx,
	long fs, long es, long ds,
	long eip, long cs, long eflags,
	unsigned long * esp, long ss)
{
}

void math_error(void)
{
}

void math_state_restore()
{
}

void math_emulate(long edi, long esi, long ebp, long sys_call_ret,
	long eax,long ebx,long ecx,long edx,
	unsigned short fs,unsigned short es,unsigned short ds,
	unsigned long eip,unsigned short cs,unsigned long eflags,
	unsigned short ss, unsigned long esp)
{
}

int do_execve(unsigned long * eip,long tmp,char * filename,
	char ** argv, char ** envp)
{
	return 0;
}

void unexpected_hd_interrupt(void)
{
	printf("Unexpected HD interrupt\n\r");
}


void unexpected_floppy_interrupt(void)
{
}

