/*
filename:	fork.c
author:		wei-coder
date:		2018-1
purpose:	fork进程的相关函数实现
*/

#if 1

#include "types.h"
#include "sched.h"
#include "errorno.h"
#include "pm.h"

long last_pid=0;

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
int copy_mem (int nr, struct task_struct *p, struct task_struct *father)
{
	ulong proc_pdt = get_free_page();
	u32 new_pdt_phy_addr = 0;
	u32 old_pdt = father->pdt;
	long size = 0;

	//old_pdt = task[nr]
	
	if(!get_mapping((pdt_t*)pdt, proc_pdt, &new_pdt_phy_addr))
	{
		return -ENOMEM;
	}
	
	p->tss->cr3 = (long)new_pdt_phy_addr;

	if (copy_page_tables (old_pdt, (ulong)proc_pdt, size))
	{				// 复制代码和数据段。
		free_page_tables ((ulong)proc_pdt, size);	// 如果出错则释放申请的内存。
		return -ENOMEM;
	}
	return 0;
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
	struct task_struct *p;
	int i;

	p = (struct task_struct *)get_free_page();	// 为新任务数据结构分配内存。
	if (!p)			// 如果内存分配出错，则返回出错码并退出。
		return -EAGAIN;
	task[nr] = p;			// 将新任务结构指针放入任务数组中。
// 其中nr 为任务号，由前面find_empty_process()返回。
	*p = *current;		/* NOTE! this doesn't copy the supervisor stack */
/* 注意！这样做不会复制超级用户的堆栈 （只复制当前进程内容）。*/ 
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
// 以下设置任务状态段TSS 所需的数据（参见列表后说明）。
	p->tss.back_link = 0;
	p->tss.esp0 = PAGE_SIZE + (long) p;	// 堆栈指针（由于是给任务结构p 分配了1 页
// 新内存，所以此时esp0 正好指向该页顶端）。
	p->tss.ss0 = 0x10;		// 堆栈段选择符（内核数据段）[??]。
	p->tss.eip = eip;		// 指令代码指针。
	p->tss.eflags = eflags;	// 标志寄存器。
	p->tss.eax = 0;
	p->tss.ecx = ecx;
	p->tss.edx = edx;
	p->tss.ebx = ebx;
	p->tss.esp = esp;
	p->tss.ebp = ebp;
	p->tss.esi = esi;
	p->tss.edi = edi;
	p->tss.es = es & 0xffff;	// 段寄存器仅16 位有效。
	p->tss.cs = cs & 0xffff;
	p->tss.ss = ss & 0xffff;
	p->tss.ds = ds & 0xffff;
	p->tss.fs = fs & 0xffff;
	p->tss.gs = gs & 0xffff;
	p->tss.ldt = _SELECTOR_LDT;	// 该新任务nr 的局部描述符表选择符（LDT 的描述符在GDT 中）。
	p->tss.io_bitmap = 0x80000000;

// 设置新任务的代码和数据段基址、限长并复制页表。如果出错（返回值不是0），则复位任务数组中
// 相应项并释放为该新任务分配的内存页。
	if (copy_mem(nr, p, current))
	{				// 返回不为0 表示出错。
		task[nr] = NULL;
		free_page ((long) p);
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

#endif
