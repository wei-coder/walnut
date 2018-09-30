/*
filename:	fork.c
author:		wei-coder
date:		2018-1
purpose:	fork进程的相关函数实现
*/

#include "types.h"
#include "errorno.h"
#include "string.h"
#include "memory.h"
#include "sched.h"

#define __LIBRARY__
#include "unistd.h"

long last_pid=0;

extern void ret_from_sys_call(void);

inline _syscall0(int,fork)


#if 0
/*
进程空间区域写前验证函数。对当前进程的地址addr 到addr+size 这一段进程空
间以页为单位执行写操作前的检测操作。若页面是只读的，则执行共享检验和复制页面操作（写时复制）。
*/
void verify_area (void *addr, int size)
{
	ulong start;

	start = (ulong) addr;
	size += start & 0xfff;
	start &= 0xfffff000;
	start += get_base (current->ldt[2]);// 此时start 变成系统整个线性空间中的地址位置。
	while (size > 0)
	{
		size -= 4096;
// 写页面验证。若页面不可写，则复制页面。（mm/memory.c，261 行）
		write_verify (start);
		start += 4096;
	}
}
#endif
// 设置新任务的代码和数据段基址、限长并复制页表。
// nr 为新任务号；p 是新任务数据结构的指针。
int copy_mem(int nr,struct task_struct * proc, struct task_struct * f_proc)
{
	proc->pdt = (u32)alloc_page();
	if(0 != proc->pdt)
	{
		memset((const void*)(proc->pdt+PAGE_OFFSET), 0, PAGE_SIZE);
	}
	
	if(copy_page_tables((u32*)(f_proc->pdt + PAGE_OFFSET), (u32*)(proc->pdt + PAGE_OFFSET)) )
	{
		return TRUE;
	}
	free_page_tables(proc->pdt);
	return -ENOMEM;

}


/*
* OK，下面是主要的fork 子程序。它复制系统进程信息(task[n])并且设置必要的寄存器。
* 它还整个地复制数据段。
*/
// 复制进程。
int copy_process (int nr, long ebp, long edi, long esi, long gs, long none,
				  long ebx, long ecx, long edx,
				  long fs, long es, long ds,
				  long eip, long cs, long eflags, long esp, long ss)
{
	struct task_struct *p = NULL;
	u32* kern_stack = NULL;

	p = (struct task_struct *)get_virt_page();	// 为新任务数据结构分配内存。
	if (!p)			// 如果内存分配出错，则返回出错码并退出。
		return -EAGAIN;
	task[nr] = p;			// 将新任务结构指针放入任务数组中。
// 其中nr 为任务号，由前面find_empty_process()返回。
	*p = *current;		/* NOTE! this doesn't copy the supervisor stack */
/* 注意！这样做不会复制内核堆栈 （只复制当前进程内容）。*/ 
	p->state = TASK_UNINTERRUPTIBLE;	// 将新进程的状态先置为不可中断等待状态。
	p->pid = last_pid;		// 新进程号。由前面调用find_empty_process()得到。
	p->father = current->pid;	// 设置父进程号。
	p->counter = p->priority;
	p->signal = 0;		// 信号位图置0。
	p->alarm = 0;
	p->leader = 0;		/* process leadership doesn't inherit */
/* 进程的领导权是不能继承的 */
	p->utime = p->stime = 0;	// 初始化用户态时间和核心态时间。
	p->cutime = p->cstime = 0;	// 初始化子进程用户态和核心态时间。
	p->start_time = jiffies;	// 当前滴答数时间。
	p->esp0 = (long)p + PAGE_SIZE;
	p->eip = eip;


	kern_stack = (u32*)((long)p + PAGE_SIZE);
	*(--kern_stack) = ss&0xFFFF;
	*(--kern_stack) = esp;
	*(--kern_stack) = eflags;
	*(--kern_stack) = cs&0xFFFF;
	*(--kern_stack) = eip;
	*(--kern_stack) = ds&0xFFFF;
	*(--kern_stack) = es&0xFFFF;
	*(--kern_stack) = fs&0xFFFF;
	*(--kern_stack) = edx;
	*(--kern_stack) = ecx;
	*(--kern_stack) = ebx;
	*(--kern_stack) = 0;
	*(--kern_stack) = (u32)ret_from_sys_call;
	*(--kern_stack) = ebp;
	*(--kern_stack) = esi;
	*(--kern_stack) = ebx;
	kern_stack -= 4;
	p->esp = (long)kern_stack;


// 设置新任务的代码和数据段基址、限长并复制页表。如果出错（返回值不是0），则复位任务数组中
// 相应项并释放为该新任务分配的内存页。
	if (FALSE == copy_mem(nr, p, current))
	{				// 返回为0 表示出错。
		task[nr] = NULL;
		free_virt_page ((u32) p);
		return -EAGAIN;
	}

// 在GDT 中设置新任务的TSS 和LDT 描述符项，数据从task 结构中取。
// 在任务切换时，任务寄存器tr 由CPU 自动加载。
	//set_tss_desc (gdt + (nr << 1) + FIRST_TSS_ENTRY, &(p->tss));
	//set_ldt_desc (gdt + (nr << 1) + FIRST_LDT_ENTRY, &(p->ldt));
	p->state = TASK_RUNNING;	/* do this last, just in case */
/* 最后再将新任务设置成可运行状态，以防万一 */
	return last_pid;		// 返回新进程号（与任务号是不同的）。
}


// 为新进程取得不重复的进程号last_pid，并返回在任务数组中的任务号(数组index)。
int find_empty_process(void)
{
    int i;

    repeat:
// 如果last_pid 增1 后超出其正数表示范围，则重新从1 开始使用pid 号。
        if ((++last_pid)<0) 
        {
            last_pid=1;
        }
// 在任务数组中搜索刚设置的pid 号是否已经被任何任务使用。如果是则重新获得一个pid 号。        
        for(i=0 ; i<NR_TASKS ; i++)
        {
            if (task[i] && task[i]->pid == last_pid)
            {
                goto repeat;
            }
        }
        
// 在任务数组中为新任务寻找一个空闲项，并返回项号。last_pid 是一个全局变量，不用返回。        
    for(i=1 ; i<NR_TASKS ; i++)
    {
        if (!task[i])
        {
            return i;
        }
    }
    return -EAGAIN;
}

