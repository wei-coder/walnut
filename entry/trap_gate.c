/*
filename:traps.c
author:linux
date:2018-1
purpose:系统陷阱门的初始化
*/

#include "trap_gate.h"
#include "console.h"
#include "sched.h"
#include "memory.h"
#include "pm.h"
#include "io.h"

// 取段seg 中地址addr 处的一个字节。
#define get_seg_byte(seg,addr) ({ \
register char __res; \
__asm__("push %%fs;mov %%ax,%%fs;movb %%fs:%2,%%al;pop %%fs" \
    :"=a" (__res):"0" (seg),"m" (*(addr))); \
__res;})

// 取段seg 中地址addr 处的一个长字（4 字节）。
#define get_seg_long(seg,addr) ({ \
register unsigned long __res; \
__asm__("push %%fs;mov %%ax,%%fs;movl %%fs:%2,%%eax;pop %%fs" \
    :"=a" (__res):"0" (seg),"m" (*(addr))); \
__res;})

// 取fs 段寄存器的值（选择符）。
#define _fs() ({ \
register unsigned short __res; \
__asm__("mov %%fs,%%ax":"=a" (__res):); \
__res;})

int do_exit(long code);                 // 程序退出处理。(kernel/exit.c)

// 该子程序用来打印出错中断的名称、出错号、调用程序的EIP、EFLAGS、ESP、fs 段寄存器值、
// 段的基址、段的长度、进程号pid、任务号、10 字节指令码。如果堆栈在用户数据段，则还
// 打印16 字节的堆栈内容。
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

// 下面是异常（陷阱）中断程序初始化子程序。设置它们的中断调用门（中断向量）。
// set_trap_gate()与set_system_gate()的主要区别在于前者设置的特权级为0，后者是3。因此
// 断点陷阱中断int3、溢出中断overflow 和边界出错中断bounds 可以由任何程序产生。
// 这两个函数均是嵌入式汇编宏程序(include/asm/system.h)。
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
// 下面将int17-48 的陷阱门先均设置为reserved，以后每个硬件初始化时会重新设置自己的陷阱门。
    for (i=17;i<48;i++)
    {
        register_int_handler(i,&do_reserved);
    }
    
    register_int_handler(45,&irq13);               // 设置协处理器的陷阱门。
    outb_p(inb_p(0x21)&0xfb,0x21);          // 允许主8259A 芯片的IRQ2 中断请求。
    outb(inb_p(0xA1)&0xdf,0xA1);            // 允许从8259A 芯片的IRQ13 中断请求。
    //register_int_handler(39,&do_parallel_interrupt);  // 设置并行口的陷阱门。
}

