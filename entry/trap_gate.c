/*
filename:traps.c
author:linux
date:2018-1
purpose:ϵͳ�����ŵĳ�ʼ��
*/

#include "trap_gate.h"
#include "console.h"
#include "sched.h"
#include "memory.h"
#include "pm.h"
#include "io.h"

// ȡ��seg �е�ַaddr ����һ���ֽڡ�
#define get_seg_byte(seg,addr) ({ \
register char __res; \
__asm__("push %%fs;mov %%ax,%%fs;movb %%fs:%2,%%al;pop %%fs" \
    :"=a" (__res):"0" (seg),"m" (*(addr))); \
__res;})

// ȡ��seg �е�ַaddr ����һ�����֣�4 �ֽڣ���
#define get_seg_long(seg,addr) ({ \
register unsigned long __res; \
__asm__("push %%fs;mov %%ax,%%fs;movl %%fs:%2,%%eax;pop %%fs" \
    :"=a" (__res):"0" (seg),"m" (*(addr))); \
__res;})

// ȡfs �μĴ�����ֵ��ѡ�������
#define _fs() ({ \
register unsigned short __res; \
__asm__("mov %%fs,%%ax":"=a" (__res):); \
__res;})

int do_exit(long code);                 // �����˳�����(kernel/exit.c)

// ���ӳ���������ӡ�����жϵ����ơ�����š����ó����EIP��EFLAGS��ESP��fs �μĴ���ֵ��
// �εĻ�ַ���εĳ��ȡ����̺�pid������š�10 �ֽ�ָ���롣�����ջ���û����ݶΣ���
// ��ӡ16 �ֽڵĶ�ջ���ݡ�
static void die(char * str,long esp_ptr,long nr)
{
	long * esp = (long *) esp_ptr;
	int i;

	printf("%s: %04x\n\r",str,nr&0xffff);
	printf("EIP:\t%04x:%p\nEFLAGS:\t%p\nESP:\t%04x:%p\n", esp[1],esp[0],esp[2],esp[4],esp[3]);
	printf("fs: %04x\n",_fs());
	//printf("base: %p, limit: %p\n",get_base(current->ldt[1]),get_limit(0x17));
	if (esp[4] == 0x17)
	{
		printf("Stack: ");
		for (i=0;i<4;i++)
		printf("%p ",get_seg_long(0x17,i+(long *)esp[3]));
		printf("\n");
	}
	//str(i);
	printf("Pid: %d, process nr: %d\n\r",current->pid,0xffff & i);
	for(i=0;i<10;i++)
	{
		printf("%02x ",0xff & get_seg_byte(esp[1],(i+(char *)esp[0])));
	}
	printf("\n\r");
	do_exit(11);        /* play segment exception */
}

void do_double_fault(int_cont_t * context)
{
    die("double fault",context->esp,context->err_code);
}

void do_general_protection(int_cont_t * context)
{
    die("general protection",context->esp,context->err_code);
}

void do_divide_error(int_cont_t * context)
{
    die("divide error",context->esp,context->err_code);
}

void do_int3(int_cont_t * context)
{
    printf("eax\t\tebx\t\tecx\t\tedx\n\r%8x\t%8x\t%8x\t%8x\n\r", context->eax,context->ebx,context->ecx,context->edx);
    printf("esi\t\tedi\t\tebp\t\tesp\n\r%8x\t%8x\t%8x\t%8x\n\r",  context->esi,context->edi,context->ebp, context->esp);
    printf("\n\rds\n\r%4x\n\r",  context->ds);
    printf("EIP: %8x   CS: %4x  EFLAGS: %8x\n\r",context->eip,context->cs,context->eflags);
}

void do_nmi(int_cont_t * context)
{
    die("nmi",context->esp,context->err_code);
}

void do_debug(int_cont_t * context)
{
    die("debug",context->esp,context->err_code);
}

void do_overflow(int_cont_t * context)
{
    die("overflow",context->esp,context->err_code);
}

void do_bounds(int_cont_t * context)
{
    die("bounds",context->esp,context->err_code);
}

void do_invalid_op(int_cont_t * context)
{
    die("invalid operand",context->esp,context->err_code);
}

void do_device_not_available(int_cont_t * context)
{
    die("device not available",context->esp,context->err_code);
}

void do_coprocessor_segment_overrun(int_cont_t * context)
{
    die("coprocessor segment overrun",context->esp,context->err_code);
}

void do_invalid_TSS(int_cont_t * context)
{
    die("invalid TSS",context->esp,context->err_code);
}

void do_segment_not_present(int_cont_t * context)
{
    die("segment not present",context->esp,context->err_code);
}

void do_stack_segment(int_cont_t * context)
{
    die("stack segment",context->esp,context->err_code);
}

void do_coprocessor_error(int_cont_t * context)
{
    if (last_task_used_math != current)
        return;
    die("coprocessor error",context->esp,context->err_code);
}

void do_reserved(int_cont_t * context)
{
    die("reserved (15,17-47) error",context->esp,context->err_code);
}

// �������쳣�����壩�жϳ����ʼ���ӳ����������ǵ��жϵ����ţ��ж���������
// set_trap_gate()��set_system_gate()����Ҫ��������ǰ�����õ���Ȩ��Ϊ0��������3�����
// �ϵ������ж�int3������ж�overflow �ͱ߽�����ж�bounds �������κγ��������
// ��������������Ƕ��ʽ�������(include/asm/system.h)��
void trap_init(void)
{
    int i;

    register_int_handler(0,&do_divide_error);
    register_int_handler(1,&do_debug);
    register_int_handler(2,&do_nmi);
    register_int_handler(3,&do_int3);   /* int3-5 can be called from all */
    register_int_handler(4,&do_overflow);
    register_int_handler(5,&do_bounds);
    register_int_handler(6,&do_invalid_op);
    register_int_handler(7,&do_device_not_available);
    register_int_handler(8,&do_double_fault);
    register_int_handler(9,&do_coprocessor_segment_overrun);
    register_int_handler(10,&do_invalid_TSS);
    register_int_handler(11,&do_segment_not_present);
    register_int_handler(12,&do_stack_segment);
    register_int_handler(13,&do_general_protection);
    //register_int_handler(14,&page_fault);
    register_int_handler(15,&do_reserved);
    register_int_handler(16,&do_coprocessor_error);
// ���潫int17-48 ���������Ⱦ�����Ϊreserved���Ժ�ÿ��Ӳ����ʼ��ʱ�����������Լ��������š�
    for (i=17;i<48;i++)
    {
        register_int_handler(i,&do_reserved);
    }
    
    register_int_handler(45,&irq13);               // ����Э�������������š�
    outb_p(inb_p(0x21)&0xfb,0x21);          // ������8259A оƬ��IRQ2 �ж�����
    outb(inb_p(0xA1)&0xdf,0xA1);            // �����8259A оƬ��IRQ13 �ж�����
    //register_int_handler(39,&do_parallel_interrupt);  // ���ò��пڵ������š�
}

