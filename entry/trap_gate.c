<<<<<<< HEAD
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

//int do_exit(long code);                 // �����˳�����(kernel/exit.c)

static void die(char * str,long esp_ptr,long nr)
{
	long * esp = (long *) esp_ptr;
	int i;

	printf("%s: %04x\n\r",str,nr&0xffff);
	printf("EIP:\t%04x:%p\nEFLAGS:\t%p\nESP:\t%04x:%p\n",esp[1],esp[0],esp[2],esp[4],esp[3]);
	printf("fs: %04x\n",_fs());
	if (esp[4] == _SELECTOR_USER_DS)
	{
		printf("Stack: ");
		for (i=0;i<4;i++)
			printf("%p ",get_seg_long(_SELECTOR_USER_DS,i+(long *)esp[3]));
		printf("\n");
	}
	//str(i);		//����TR�Ĵ���
	printf("Pid: %d, process nr: %d\n\r",current->pid,0xffff & i);
	for(i=0;i<10;i++)
		printf("%02x ",0xff & get_seg_byte(esp[1],(i+(char *)esp[0])));
	printf("\n\r");
	//do_exit(11);        /* play segment exception */
}

void do_double_fault(long esp, long error_code)
{
    die("double fault",esp,error_code);
}

void do_general_protection(long esp, long error_code)
{
    die("general protection",esp,error_code);
}

void do_divide_error(long esp, long error_code)
{
    die("divide error",esp,error_code);
}

void do_int3(long * esp, long error_code,
		long fs,long es,long ds,
		long ebp,long esi,long edi,
		long edx,long ecx,long ebx,long eax)
{
    int tr;

    __asm__("str %%ax":"=a" (tr):"0" (0));
    printf("eax\t\tebx\t\tecx\t\tedx\n\r%8x\t%8x\t%8x\t%8x\n\r",
        eax,ebx,ecx,edx);
    printf("esi\t\tedi\t\tebp\t\tesp\n\r%8x\t%8x\t%8x\t%8x\n\r",
        esi,edi,ebp,(long) esp);
    printf("\n\rds\tes\tfs\ttr\n\r%4x\t%4x\t%4x\t%4x\n\r",
        ds,es,fs,tr);
    printf("EIP: %8x   CS: %4x  EFLAGS: %8x\n\r",esp[0],esp[1],esp[2]);
}

void do_nmi(long esp, long error_code)
{
    die("nmi",esp,error_code);
}

void do_debug(long esp, long error_code)
{
    die("debug",esp,error_code);
}

void do_overflow(long esp, long error_code)
{
    die("overflow",esp,error_code);
}

void do_bounds(long esp, long error_code)
{
    die("bounds",esp,error_code);
}

void do_invalid_op(long esp, long error_code)
{
    die("invalid operand",esp,error_code);
}

void do_device_not_available(long esp, long error_code)
{
    die("device not available",esp,error_code);
}

void do_coprocessor_segment_overrun(long esp, long error_code)
{
    die("coprocessor segment overrun",esp,error_code);
}

void do_invalid_TSS(long esp,long error_code)
{
    die("invalid TSS",esp,error_code);
}

void do_segment_not_present(long esp,long error_code)
{
    die("segment not present",esp,error_code);
}

void do_stack_segment(long esp,long error_code)
{
    die("stack segment",esp,error_code);
}

void do_coprocessor_error(long esp, long error_code)
{
    if (last_task_used_math != current)
        return;
    die("coprocessor error",esp,error_code);
}

void do_reserved(long esp, long error_code)
{
    die("reserved (15,17-47) error",esp,error_code);
}

#if 0
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
#endif
=======
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

//int do_exit(long code);                 // �����˳�����(kernel/exit.c)

static void die(char * str,long esp_ptr,long nr)
{
	long * esp = (long *) esp_ptr;
	int i;

	printf("%s: %04x\n\r",str,nr&0xffff);
	printf("EIP:\t%04x:%p\nEFLAGS:\t%p\nESP:\t%04x:%p\n",esp[1],esp[0],esp[2],esp[4],esp[3]);
	printf("fs: %04x\n",_fs());
	if (esp[4] == _SELECTOR_USER_DS)
	{
		printf("Stack: ");
		for (i=0;i<4;i++)
			printf("%p ",get_seg_long(_SELECTOR_USER_DS,i+(long *)esp[3]));
		printf("\n");
	}
	//str(i);		//����TR�Ĵ���
	printf("Pid: %d, process nr: %d\n\r",current->pid,0xffff & i);
	for(i=0;i<10;i++)
		printf("%02x ",0xff & get_seg_byte(esp[1],(i+(char *)esp[0])));
	printf("\n\r");
	//do_exit(11);        /* play segment exception */
}

void do_double_fault(long esp, long error_code)
{
    die("double fault",esp,error_code);
}

void do_general_protection(long esp, long error_code)
{
    die("general protection",esp,error_code);
}

void do_divide_error(long esp, long error_code)
{
    die("divide error",esp,error_code);
}

void do_int3(long * esp, long error_code,
		long fs,long es,long ds,
		long ebp,long esi,long edi,
		long edx,long ecx,long ebx,long eax)
{
    int tr;

    __asm__("str %%ax":"=a" (tr):"0" (0));
    printf("eax\t\tebx\t\tecx\t\tedx\n\r%8x\t%8x\t%8x\t%8x\n\r",
        eax,ebx,ecx,edx);
    printf("esi\t\tedi\t\tebp\t\tesp\n\r%8x\t%8x\t%8x\t%8x\n\r",
        esi,edi,ebp,(long) esp);
    printf("\n\rds\tes\tfs\ttr\n\r%4x\t%4x\t%4x\t%4x\n\r",
        ds,es,fs,tr);
    printf("EIP: %8x   CS: %4x  EFLAGS: %8x\n\r",esp[0],esp[1],esp[2]);
}

void do_nmi(long esp, long error_code)
{
    die("nmi",esp,error_code);
}

void do_debug(long esp, long error_code)
{
    die("debug",esp,error_code);
}

void do_overflow(long esp, long error_code)
{
    die("overflow",esp,error_code);
}

void do_bounds(long esp, long error_code)
{
    die("bounds",esp,error_code);
}

void do_invalid_op(long esp, long error_code)
{
    die("invalid operand",esp,error_code);
}

void do_device_not_available(long esp, long error_code)
{
    die("device not available",esp,error_code);
}

void do_coprocessor_segment_overrun(long esp, long error_code)
{
    die("coprocessor segment overrun",esp,error_code);
}

void do_invalid_TSS(long esp,long error_code)
{
    die("invalid TSS",esp,error_code);
}

void do_segment_not_present(long esp,long error_code)
{
    die("segment not present",esp,error_code);
}

void do_stack_segment(long esp,long error_code)
{
    die("stack segment",esp,error_code);
}

void do_coprocessor_error(long esp, long error_code)
{
    if (last_task_used_math != current)
        return;
    die("coprocessor error",esp,error_code);
}

void do_reserved(long esp, long error_code)
{
    die("reserved (15,17-47) error",esp,error_code);
}

#if 0
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
#endif
>>>>>>> 2747b789ab05c6901e2e08b9db7fbb72fd8f6f24
