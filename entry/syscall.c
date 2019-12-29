/*
filename:	syscall.c
author:		��ʱ������linux0.11��ʵ��
date:		2018-1
purpose:	ϵͳ���õ���غ���ʵ��
*/
#include <kio.h>
#define __LIBRARY__
#include <unistd.h>
#include <utsname.h>
#include "../vfs/vfs.h"
#include "../mm/memory.h"
#include "../driver/timer.h"
#include "utsname.h"
#include "errorno.h"
#include "syscall.h"

extern long volatile jiffies;							// �ӿ�����ʼ����ĵδ���ʱ��ֵ��10ms/�δ𣩡�
extern long startup_time;								// ����ʱ�䡣��1970:0:0:0 ��ʼ��ʱ��������
extern struct task_struct *current;						// ��ǰ����ָ�루��ʼ��Ϊ��ʼ���񣩡�
extern struct task_struct *task[NR_TASKS];				// ��������ָ�����顣

ulong do_hd;
ulong do_floppy;

_syscall1(int,uname,struct utsname *,utsbuf);


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
	if (!cpl){
		//printf("do_timer current task_%ld counter: %ld cpl:%d\n", current->pid, current->counter, cpl);
		return;			// ���ڳ����û����򣬲�����counter ֵ���е��ȡ�
	}
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

int sys_waitpid()
{
	return 0;
}

int sys_exit(int error_code)
{
	return -1;
}

int sys_pause()
{
	return 0;
}

int sys_utime()
{
	return 0;
}

int sys_access()
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

int sys_dup()
{
	return 0;
}

int sys_pipe()
{
	return 0;
}

int sys_signal()
{
	return 0;
}

int sys_ioctl()
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
	printf("Unexpected HD interrupt!\n");
}


void unexpected_floppy_interrupt(void)
{
}

int sys_ftime()
{
    return -ENOSYS;
}

int sys_break()
{
    return -ENOSYS;
}

int sys_ptrace()
{
    return -ENOSYS;
}

int sys_stty()
{
    return -ENOSYS;
}

int sys_gtty()
{
    return -ENOSYS;
}

int sys_prof()
{
    return -ENOSYS;
}

// ���õ�ǰ�����ʵ���Լ�/������Ч��ID��gid�����������û�г����û���Ȩ��
// ��ôֻ�ܻ�����ʵ����ID ����Ч��ID�����������г����û���Ȩ����������������Ч�ĺ�ʵ��
// ����ID��������gid��saved gid�������ó�����Чgid ֵͬ��
int sys_setregid(int rgid, int egid)
{
    if (rgid>0) 
    {
        if ( (current->gid == rgid)  )
        {
            current->gid = rgid;
        }
        else
        {
            return(-EPERM);
        }
    }
    
    if (egid>0) 
    {
        if ( (current->gid == egid) 
          || (current->egid == egid) 
          || (current->sgid == egid) )
        {
            current->egid = egid;
        }
        else
        {
            return(-EPERM);
        }
    }
    return 0;
}

// ���ý������(gid)���������û�г����û���Ȩ��������ʹ��setgid()������Чgid
// ��effective gid������Ϊ���䱣��gid(saved gid)����ʵ��gid(real gid)�����������
// �����û���Ȩ����ʵ��gid����Чgid �ͱ���gid �������óɲ���ָ����gid��
int sys_setgid(int gid)
{
    return(sys_setregid(gid, gid));
}

int sys_acct()
{
    return -ENOSYS;
}

int sys_phys()
{
    return -ENOSYS;
}

int sys_lock()
{
    return -ENOSYS;
};

int sys_mpx()
{
    return -ENOSYS;
};

int sys_ulimit()
{
    return -ENOSYS;
};

/*
 * Unprivileged users may change the real user id to the effective uid
 * or vice versa.
 */
/*
* ����Ȩ���û����Խ�ʵ���û���ʶ��(real uid)�ĳ���Ч�û���ʶ��(effective uid)����֮ҲȻ��
*/
// ���������ʵ���Լ�/������Ч�û�ID��uid�����������û�г����û���Ȩ����ôֻ�ܻ�����
// ʵ���û�ID ����Ч�û�ID�����������г����û���Ȩ����������������Ч�ĺ�ʵ�ʵ��û�ID��
// ������uid��saved uid�������ó�����Чuid ֵͬ��
int sys_setreuid(int ruid, int euid)
{
    int old_ruid = current->uid;
    
    if (ruid>0) 
    {
        if ( (current->euid==ruid) 
          || (old_ruid == ruid))
        {
            current->uid = ruid;
        }
        else
        {
            return(-EPERM);
        }
    }
    
    if (euid>0) 
    {
        if ( (old_ruid == euid) 
          || (current->euid == euid))
        {
            current->euid = euid;
        }
        else 
        {
            current->uid = old_ruid;
            return(-EPERM);
        }
    }
    return 0;
};

// ���������û���(uid)���������û�г����û���Ȩ��������ʹ��setuid()������Чuid
// ��effective uid�����ó��䱣��uid(saved uid)����ʵ��uid(real uid)�����������
// �����û���Ȩ����ʵ��uid����Чuid �ͱ���uid �������óɲ���ָ����uid��
int sys_setuid(int uid)
{
    return(sys_setreuid(uid, uid));
};

// ����ϵͳʱ������ڡ�����tptr �Ǵ�1970 ��1 ��1 ��00:00:00 GMT ��ʼ��ʱ��ʱ��ֵ���룩��
// ���ý��̱�����г����û�Ȩ�ޡ�
// ������⣬startup_timeΪ����ʱ��
int sys_stime(long * tptr)
{
    return 0;
}

// ��ȡ��ǰ����ʱ�䡣tms �ṹ�а����û�ʱ�䡢ϵͳʱ�䡢�ӽ����û�ʱ�䡢�ӽ���ϵͳʱ�䡣
int sys_times(struct tms * tbuf)
{
    return jiffies;
};

// ������end_data_seg ��ֵ��������ϵͳȷʵ���㹻���ڴ棬���ҽ���û�г�Խ��������ݶδ�С
// ʱ���ú����������ݶ�ĩβΪend_data_seg ָ����ֵ����ֵ������ڴ����β����ҪС�ڶ�ջ
// ��β16KB������ֵ�����ݶε��½�βֵ���������ֵ��Ҫ��ֵ��ͬ��������д�������
// �ú����������û�ֱ�ӵ��ã�����libc �⺯�����а�װ�����ҷ���ֵҲ��һ����
int sys_brk(unsigned long end_data_seg)
{
    if (end_data_seg >= current->end_code &&
        end_data_seg < current->start_stack - 16384)
    {
        current->brk = end_data_seg;
    }
    
    return current->brk;
};

/*
 * This needs some heave checking ...
 * I just haven't get the stomach for it. I also don't fully
 * understand sessions/pgrp etc. Let somebody who does explain it.
 */
// ���ý��̵Ľ�����ID Ϊpgid��
// �������pid=0����ʹ�õ�ǰ���̺š����pgid Ϊ0����ʹ�ò���pid ָ���Ľ��̵���ID ��Ϊ
// pgid������ú������ڽ����̴�һ���������Ƶ���һ�������飬���������������������ͬһ��
// �Ự(session)������������£�����pgid ָ����Ҫ��������н�����ID����ʱ����ĻỰID
// �����뽫Ҫ������̵���ͬ��
int sys_setpgid(int pid, int pgid)
{
	int i;

	if (!pid)
	{
		pid = current->pid;
	}

	if (!pgid)
	{
		pgid = current->pid;
	}

	// ɨ���������飬����ָ�����̺ŵ�����    
	for (i=0 ; i<NR_TASKS ; i++)
	{
		if (task[i] && task[i]->pid==pid) 
		{
			// ����������Ѿ������죬������ء�
			if (task[i]->leader)
			{
				return -EPERM;
			}

			if (task[i]->session != current->session)
			{
				return -EPERM;
			}

			task[i]->pgrp = pgid;
			return 0;
		}
	}
	return -ESRCH;
}

int sys_getpgrp(void)
{
    return current->pgrp;
}

// ����һ���Ự(session)����������leader=1��������������Ự��=�����=����̺š�
// setsid -- SET Session ID��
int sys_setsid(void)
{
// �����ǰ�������ǻỰ���첢�Ҳ��ǳ����û�������ء�
    if (current->leader)
    {
        return -EPERM;
    }
    // ���õ�ǰ����Ϊ�»Ự���졣
    current->leader = 1;
    // ���ñ�����session = pid��
    current->session = current->pgrp = current->pid;
    // ��ʾ��ǰ����û�п����նˡ�
    current->tty = -1;
    return current->pgrp;
}

// ��ȡϵͳ��Ϣ������utsname �ṹ����5 ���ֶΣ��ֱ��ǣ����汾����ϵͳ�����ơ�����ڵ����ơ�
// ��ǰ���м��𡢰汾�����Ӳ���������ơ�
int sys_uname(struct utsname * name)
{
	if(NULL == name)
	{
		return -1;
	}
	sprintf(name->machine, "x86-32");
	sprintf(name->nodename,"N/A");
	sprintf(name->release, "r1");
	sprintf(name->sysname, "walnut");
	sprintf(name->version, "v1");
    return 0;
}

// ���õ�ǰ���̴����ļ�����������Ϊmask & 0777��������ԭ�����롣
int sys_umask(int mask)
{
    int old = current->umask;

    current->umask = mask & 0777;
    return (old);
}
