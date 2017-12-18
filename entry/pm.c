/*
filename:	pm.c
author:	wei-coder
date:	2017-12
purpose:	系统进入保护模式所需要的各项工作
*/
#include "pm.h"

/*全局描述符表的定义*/
glb_desc_t gdt_entry[GDT_ENTRY_LEN];

/*中断描述符表的定义*/
gate_desc_t idt_entry[IDT_ENTRY_LEN];

/*中断处理函数表的定义*/
int_handler_ptr isr_entry[IDT_ENTRY_LEN];


/*全局描述符表的初始化*/
void init_gdt()
{
	struct gdtr_t GDTR = {0};
	GDTR.length = sizeof(glb_desc_t)*GDT_ENTRY_LEN - 1;
	GDTR.base_addr = (u32)&gdt_entry;
	
	gdt_entry[GDT_ENTRY_NULL] = GLB_DESC_INIT(0, 0, 0);
	gdt_entry[GDT_ENTRY_RESEVE_1] = GLB_DESC_INIT(0, 0, 0);
	gdt_entry[GDT_ENTRY_KERNEL_CS] = GLB_DESC_INIT(KERNEL_CS_FLAG, KERNEL_CS_BASE, KERNEL_CS_LIMIT);
	gdt_entry[GDT_ENTRY_KERNEL_DS] =GLB_DESC_INIT(KERNEL_DS_FLAG, KERNEL_DS_BASE, KERNEL_DS_LIMIT);
	gdt_entry[GDT_ENTRY_USER_CS] = GLB_DESC_INIT(USER_CS_FLAG, USER_CS_BASE, USER_CS_LIMIT);
	gdt_entry[GDT_ENTRY_USER_DS] = GLB_DESC_INIT(USER_DS_FLAG, USER_DS_BASE, USER_DS_LIMIT);
	
	asm volatile("lgdtl %0" : : "m" (GDTR));
};

/*中断描述符表的初始化*/
void init_idt()
{
	/*idtr的初始化*/
	struct idtr_t IDTR = {0};
	IDTR.length = sizeof(glb_desc_t)*IDT_ENTRY_LEN -1;
	IDTR.base = (u32)&idt_entry;
	
	/*中断描述符表的初始化*/
	idt_entry[0] = INT_DESC_INIT((u32)isr0, 0x08, 0x8E);
	idt_entry[1] = INT_DESC_INIT((u32)isr1, 0x08, 0x8E);
	idt_entry[2] = INT_DESC_INIT((u32)isr2, 0x08, 0x8E);
	idt_entry[3] = INT_DESC_INIT((u32)isr3, 0x08, 0x8E);
	idt_entry[4] = INT_DESC_INIT((u32)isr4, 0x08, 0x8E);
	idt_entry[5] = INT_DESC_INIT((u32)isr5, 0x08, 0x8E);
	idt_entry[6] = INT_DESC_INIT((u32)isr6, 0x08, 0x8E);
	idt_entry[7] = INT_DESC_INIT((u32)isr7, 0x08, 0x8E);
	idt_entry[8] = INT_DESC_INIT((u32)isr8, 0x08, 0x8E);
	idt_entry[9] = INT_DESC_INIT((u32)isr9, 0x08, 0x8E);
	idt_entry[10] = INT_DESC_INIT((u32)isr10, 0x08, 0x8E);
	idt_entry[11] = INT_DESC_INIT((u32)isr11, 0x08, 0x8E);
	idt_entry[12] = INT_DESC_INIT((u32)isr12, 0x08, 0x8E);
	idt_entry[13] = INT_DESC_INIT((u32)isr13, 0x08, 0x8E);
	idt_entry[14] = INT_DESC_INIT((u32)isr14, 0x08, 0x8E);
	idt_entry[15] = INT_DESC_INIT((u32)isr15, 0x08, 0x8E);
	idt_entry[16] = INT_DESC_INIT((u32)isr16, 0x08, 0x8E);
	idt_entry[17] = INT_DESC_INIT((u32)isr17, 0x08, 0x8E);
	idt_entry[18] = INT_DESC_INIT((u32)isr18, 0x08, 0x8E);
	idt_entry[19] = INT_DESC_INIT((u32)isr19, 0x08, 0x8E);
	idt_entry[20] = INT_DESC_INIT((u32)isr20, 0x08, 0x8E);
	idt_entry[21] = INT_DESC_INIT((u32)isr21, 0x08, 0x8E);
	idt_entry[22] = INT_DESC_INIT((u32)isr22, 0x08, 0x8E);
	idt_entry[23] = INT_DESC_INIT((u32)isr23, 0x08, 0x8E);
	idt_entry[24] = INT_DESC_INIT((u32)isr24, 0x08, 0x8E);
	idt_entry[25] = INT_DESC_INIT((u32)isr25, 0x08, 0x8E);
	idt_entry[26] = INT_DESC_INIT((u32)isr26, 0x08, 0x8E);
	idt_entry[27] = INT_DESC_INIT((u32)isr27, 0x08, 0x8E);
	idt_entry[28] = INT_DESC_INIT((u32)isr28, 0x08, 0x8E);
	idt_entry[29] = INT_DESC_INIT((u32)isr29, 0x08, 0x8E);
	idt_entry[30] = INT_DESC_INIT((u32)isr30, 0x08, 0x8E);
	idt_entry[31] = INT_DESC_INIT((u32)isr31, 0x08, 0x8E);

	asm volatile("lidtl %0" : : "m" (IDTR));
};

/**/

/*注册一个中断处理函数*/
void register_int_handler(u8 num, int_handler_ptr func)
{
	idt_entry[30] = INT_DESC_INIT((u32)func, 0x08, 0x8E);
};

/*调用中断处理函数*/
void isr_handler(int_cont_t * context);

/*IRQ 处理函数*/
void irq_handler(int_cont_t *context);

/*初始化中断描述符表*/
void init_idt();

