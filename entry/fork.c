/*
filename:	fork.c
author:		wei-coder
date:		2018-1
purpose:	fork���̵���غ���ʵ��
*/

#if 1

#include "types.h"
#include "sched.h"
#include "errorno.h"
#include "pm.h"

long last_pid=0;

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
	{				// ���ƴ�������ݶΡ�
		free_page_tables ((ulong)proc_pdt, size);	// ����������ͷ�������ڴ档
		return -ENOMEM;
	}
	return 0;
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
	struct task_struct *p;
	int i;

	p = (struct task_struct *)get_free_page();	// Ϊ���������ݽṹ�����ڴ档
	if (!p)			// ����ڴ��������򷵻س����벢�˳���
		return -EAGAIN;
	task[nr] = p;			// ��������ṹָ��������������С�
// ����nr Ϊ����ţ���ǰ��find_empty_process()���ء�
	*p = *current;		/* NOTE! this doesn't copy the supervisor stack */
/* ע�⣡���������Ḵ�Ƴ����û��Ķ�ջ ��ֻ���Ƶ�ǰ�������ݣ���*/ 
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
// ������������״̬��TSS ��������ݣ��μ��б��˵������
	p->tss.back_link = 0;
	p->tss.esp0 = PAGE_SIZE + (long) p;	// ��ջָ�루�����Ǹ�����ṹp ������1 ҳ
// ���ڴ棬���Դ�ʱesp0 ����ָ���ҳ���ˣ���
	p->tss.ss0 = 0x10;		// ��ջ��ѡ������ں����ݶΣ�[??]��
	p->tss.eip = eip;		// ָ�����ָ�롣
	p->tss.eflags = eflags;	// ��־�Ĵ�����
	p->tss.eax = 0;
	p->tss.ecx = ecx;
	p->tss.edx = edx;
	p->tss.ebx = ebx;
	p->tss.esp = esp;
	p->tss.ebp = ebp;
	p->tss.esi = esi;
	p->tss.edi = edi;
	p->tss.es = es & 0xffff;	// �μĴ�����16 λ��Ч��
	p->tss.cs = cs & 0xffff;
	p->tss.ss = ss & 0xffff;
	p->tss.ds = ds & 0xffff;
	p->tss.fs = fs & 0xffff;
	p->tss.gs = gs & 0xffff;
	p->tss.ldt = _SELECTOR_LDT;	// ��������nr �ľֲ���������ѡ�����LDT ����������GDT �У���
	p->tss.io_bitmap = 0x80000000;

// ����������Ĵ�������ݶλ�ַ���޳�������ҳ�������������ֵ����0������λ����������
// ��Ӧ��ͷ�Ϊ�������������ڴ�ҳ��
	if (copy_mem(nr, p, current))
	{				// ���ز�Ϊ0 ��ʾ����
		task[nr] = NULL;
		free_page ((long) p);
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

#endif
