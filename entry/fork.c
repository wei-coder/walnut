/*
filename:	fork.c
author:		wei-coder
date:		2018-1
purpose:	fork���̵���غ���ʵ��
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
���̿ռ�����дǰ��֤�������Ե�ǰ���̵ĵ�ַaddr ��addr+size ��һ�ν��̿�
����ҳΪ��λִ��д����ǰ�ļ���������ҳ����ֻ���ģ���ִ�й������͸���ҳ�������дʱ���ƣ���
*/
void verify_area (void *addr, int size)
{
	ulong start;

	start = (ulong) addr;
	size += start & 0xfff;
	start &= 0xfffff000;
	start += get_base (current->ldt[2]);// ��ʱstart ���ϵͳ�������Կռ��еĵ�ַλ�á�
	while (size > 0)
	{
		size -= 4096;
// дҳ����֤����ҳ�治��д������ҳ�档��mm/memory.c��261 �У�
		write_verify (start);
		start += 4096;
	}
}
#endif
// ����������Ĵ�������ݶλ�ַ���޳�������ҳ��
// nr Ϊ������ţ�p �����������ݽṹ��ָ�롣
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
* OK����������Ҫ��fork �ӳ���������ϵͳ������Ϣ(task[n])�������ñ�Ҫ�ļĴ�����
* ���������ظ������ݶΡ�
*/
// ���ƽ��̡�
int copy_process (int nr, long ebp, long edi, long esi, long gs, long none,
				  long ebx, long ecx, long edx,
				  long fs, long es, long ds,
				  long eip, long cs, long eflags, long esp, long ss)
{
	struct task_struct *p = NULL;
	u32* kern_stack = NULL;

	p = (struct task_struct *)get_virt_page();	// Ϊ���������ݽṹ�����ڴ档
	if (!p)			// ����ڴ��������򷵻س����벢�˳���
		return -EAGAIN;
	task[nr] = p;			// ��������ṹָ��������������С�
// ����nr Ϊ����ţ���ǰ��find_empty_process()���ء�
	*p = *current;		/* NOTE! this doesn't copy the supervisor stack */
/* ע�⣡���������Ḵ���ں˶�ջ ��ֻ���Ƶ�ǰ�������ݣ���*/ 
	p->state = TASK_UNINTERRUPTIBLE;	// ���½��̵�״̬����Ϊ�����жϵȴ�״̬��
	p->pid = last_pid;		// �½��̺š���ǰ�����find_empty_process()�õ���
	p->father = current->pid;	// ���ø����̺š�
	p->counter = p->priority;
	p->signal = 0;		// �ź�λͼ��0��
	p->alarm = 0;
	p->leader = 0;		/* process leadership doesn't inherit */
/* ���̵��쵼Ȩ�ǲ��ܼ̳е� */
	p->utime = p->stime = 0;	// ��ʼ���û�̬ʱ��ͺ���̬ʱ�䡣
	p->cutime = p->cstime = 0;	// ��ʼ���ӽ����û�̬�ͺ���̬ʱ�䡣
	p->start_time = jiffies;	// ��ǰ�δ���ʱ�䡣
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


// ����������Ĵ�������ݶλ�ַ���޳�������ҳ�������������ֵ����0������λ����������
// ��Ӧ��ͷ�Ϊ�������������ڴ�ҳ��
	if (FALSE == copy_mem(nr, p, current))
	{				// ����Ϊ0 ��ʾ����
		task[nr] = NULL;
		free_virt_page ((u32) p);
		return -EAGAIN;
	}

// ��GDT �������������TSS ��LDT ����������ݴ�task �ṹ��ȡ��
// �������л�ʱ������Ĵ���tr ��CPU �Զ����ء�
	//set_tss_desc (gdt + (nr << 1) + FIRST_TSS_ENTRY, &(p->tss));
	//set_ldt_desc (gdt + (nr << 1) + FIRST_LDT_ENTRY, &(p->ldt));
	p->state = TASK_RUNNING;	/* do this last, just in case */
/* ����ٽ����������óɿ�����״̬���Է���һ */
	return last_pid;		// �����½��̺ţ���������ǲ�ͬ�ģ���
}


// Ϊ�½���ȡ�ò��ظ��Ľ��̺�last_pid�������������������е������(����index)��
int find_empty_process(void)
{
    int i;

    repeat:
// ���last_pid ��1 �󳬳���������ʾ��Χ�������´�1 ��ʼʹ��pid �š�
        if ((++last_pid)<0) 
        {
            last_pid=1;
        }
// ���������������������õ�pid ���Ƿ��Ѿ����κ�����ʹ�á�����������»��һ��pid �š�        
        for(i=0 ; i<NR_TASKS ; i++)
        {
            if (task[i] && task[i]->pid == last_pid)
            {
                goto repeat;
            }
        }
        
// ������������Ϊ������Ѱ��һ���������������š�last_pid ��һ��ȫ�ֱ��������÷��ء�        
    for(i=1 ; i<NR_TASKS ; i++)
    {
        if (!task[i])
        {
            return i;
        }
    }
    return -EAGAIN;
}

