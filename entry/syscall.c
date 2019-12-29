/*
filename:	syscall.c
author:		暂时借用了linux0.11的实现
date:		2018-1
purpose:	系统调用的相关函数实现
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

extern long volatile jiffies;							// 从开机开始算起的滴答数时间值（10ms/滴答）。
extern long startup_time;								// 开机时间。从1970:0:0:0 开始计时的秒数。
extern struct task_struct *current;						// 当前任务指针（初始化为初始任务）。
extern struct task_struct *task[NR_TASKS];				// 定义任务指针数组。

ulong do_hd;
ulong do_floppy;

_syscall1(int,uname,struct utsname *,utsbuf);


//// 时钟中断C 函数处理程序，在kernel/system_call.s 中的_timer_interrupt（176 行）被调用。
// 参数cpl 是当前特权级0 或3，0 表示内核代码在执行。
// 对于一个进程由于执行时间片用完时，则进行任务切换。并执行一个计时更新工作。
void do_timer (long cpl)
{
  // 如果当前特权级(cpl)为0（最高，表示是内核程序在工作），则将超级用户运行时间stime 递增；
  // 如果cpl > 0，则表示是一般用户程序在工作，增加utime。
	if (cpl)
		current->utime++;
	else
		current->stime++;

	if ((--current->counter) > 0)
		return;			// 如果进程运行时间还没完，则退出。
	current->counter = 0;
	if (!cpl){
		//printf("do_timer current task_%ld counter: %ld cpl:%d\n", current->pid, current->counter, cpl);
		return;			// 对于超级用户程序，不依赖counter 值进行调度。
	}
	schedule();
}

// 系统调用功能 - 设置报警定时时间值(秒)。
// 如果已经设置过alarm 值，则返回旧值，否则返回0。
int sys_alarm (long seconds)
{
	int old = current->alarm;

	if (old)
		old = (old - jiffies) / HZ;
	current->alarm = (seconds > 0) ? (jiffies + HZ * seconds) : 0;
	return (old);
}

// 取当前进程号pid。
int sys_getpid (void)
{
	return current->pid;
}

// 取父进程号ppid。
int sys_getppid (void)
{
	return current->father;
}

// 取用户号uid。
int sys_getuid (void)
{
	return current->uid;
}

// 取euid。
int sys_geteuid (void)
{
	return current->euid;
}

// 取组号gid。
int sys_getgid (void)
{
	return current->gid;
}

// 取egid。
int sys_getegid (void)
{
	return current->egid;
}

// 系统调用功能 -- 降低对CPU 的使用优先权（有人会用吗？?）。
// 应该限制increment 大于0，否则的话,可使优先权增大！！
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

// 设置当前任务的实际以及/或者有效组ID（gid）。如果任务没有超级用户特权，
// 那么只能互换其实际组ID 和有效组ID。如果任务具有超级用户特权，就能任意设置有效的和实际
// 的组ID。保留的gid（saved gid）被设置成与有效gid 同值。
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

// 设置进程组号(gid)。如果任务没有超级用户特权，它可以使用setgid()将其有效gid
// （effective gid）设置为成其保留gid(saved gid)或其实际gid(real gid)。如果任务有
// 超级用户特权，则实际gid、有效gid 和保留gid 都被设置成参数指定的gid。
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
* 无特权的用户可以将实际用户标识符(real uid)改成有效用户标识符(effective uid)，反之也然。
*/
// 设置任务的实际以及/或者有效用户ID（uid）。如果任务没有超级用户特权，那么只能互换其
// 实际用户ID 和有效用户ID。如果任务具有超级用户特权，就能任意设置有效的和实际的用户ID。
// 保留的uid（saved uid）被设置成与有效uid 同值。
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

// 设置任务用户号(uid)。如果任务没有超级用户特权，它可以使用setuid()将其有效uid
// （effective uid）设置成其保留uid(saved uid)或其实际uid(real uid)。如果任务有
// 超级用户特权，则实际uid、有效uid 和保留uid 都被设置成参数指定的uid。
int sys_setuid(int uid)
{
    return(sys_setreuid(uid, uid));
};

// 设置系统时间和日期。参数tptr 是从1970 年1 月1 日00:00:00 GMT 开始计时的时间值（秒）。
// 调用进程必须具有超级用户权限。
// 个人理解，startup_time为开机时间
int sys_stime(long * tptr)
{
    return 0;
}

// 获取当前任务时间。tms 结构中包括用户时间、系统时间、子进程用户时间、子进程系统时间。
int sys_times(struct tms * tbuf)
{
    return jiffies;
};

// 当参数end_data_seg 数值合理，并且系统确实有足够的内存，而且进程没有超越其最大数据段大小
// 时，该函数设置数据段末尾为end_data_seg 指定的值。该值必须大于代码结尾并且要小于堆栈
// 结尾16KB。返回值是数据段的新结尾值（如果返回值与要求值不同，则表明有错发生）。
// 该函数并不被用户直接调用，而由libc 库函数进行包装，并且返回值也不一样。
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
// 设置进程的进程组ID 为pgid。
// 如果参数pid=0，则使用当前进程号。如果pgid 为0，则使用参数pid 指定的进程的组ID 作为
// pgid。如果该函数用于将进程从一个进程组移到另一个进程组，则这两个进程组必须属于同一个
// 会话(session)。在这种情况下，参数pgid 指定了要加入的现有进程组ID，此时该组的会话ID
// 必须与将要加入进程的相同。
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

	// 扫描任务数组，查找指定进程号的任务。    
	for (i=0 ; i<NR_TASKS ; i++)
	{
		if (task[i] && task[i]->pid==pid) 
		{
			// 如果该任务已经是首领，则出错返回。
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

// 创建一个会话(session)（即设置其leader=1），并且设置其会话号=其组号=其进程号。
// setsid -- SET Session ID。
int sys_setsid(void)
{
// 如果当前进程已是会话首领并且不是超级用户则出错返回。
    if (current->leader)
    {
        return -EPERM;
    }
    // 设置当前进程为新会话首领。
    current->leader = 1;
    // 设置本进程session = pid。
    current->session = current->pgrp = current->pid;
    // 表示当前进程没有控制终端。
    current->tty = -1;
    return current->pgrp;
}

// 获取系统信息。其中utsname 结构包含5 个字段，分别是：本版本操作系统的名称、网络节点名称、
// 当前发行级别、版本级别和硬件类型名称。
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

// 设置当前进程创建文件属性屏蔽码为mask & 0777。并返回原屏蔽码。
int sys_umask(int mask)
{
    int old = current->umask;

    current->umask = mask & 0777;
    return (old);
}
