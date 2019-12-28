/*
filename:	pm.c
author:	wei-coder
date:	2017-12
purpose:	系统进入保护模式所需要的各项工作
*/
#include <kio.h>
#include <string.h>
#include <system.h>
#include "../util/logging.h"
#include "trap_gate.h"
#include "pm.h"


/*全局描述符表的定义*/
desc_t gdt_entry[GDT_ENTRY_LEN] = {0};

/*中断描述符表的定义及初始化*/
gate_desc_t idt_entry[IDT_ENTRY_LEN] = {0};

void gdt_set_gate(u8 num, u16 flags, u32 base, u32 limit)
{
	gdt_entry[num].desc_low = ((limit) & 0xffff) | (((base) & 0xffff) << 16);
	gdt_entry[num].desc_high = (((base) & 0x00ff0000) >> 16)| (((flags) & 0xf0ff) << 8) | \
			((limit) & 0xf0000) | ((base) & 0xff000000);
};


/*全局描述符表的初始化*/
void init_gdt()
{
	struct gdtr_t gdtr_reg = {0};
	gdtr_reg.length = sizeof(desc_t)*GDT_ENTRY_LEN - 1;
	gdtr_reg.base_addr = (u32)&gdt_entry;
	
	/*全局描述符表的定义*/
	memset(gdt_entry, 0, 8*GDT_ENTRY_LEN);
	gdt_set_gate(GDT_INDEX_NULL,0, 0, 0);
	gdt_set_gate(GDT_INDEX_KERNEL_CS, KERNEL_CS_FLAG, KERNEL_CS_BASE, KERNEL_CS_LIMIT);
	gdt_set_gate(GDT_INDEX_KERNEL_DS, KERNEL_DS_FLAG, KERNEL_DS_BASE, KERNEL_DS_LIMIT);
	gdt_set_gate(GDT_INDEX_USER_CS, USER_CS_FLAG, USER_CS_BASE, USER_CS_LIMIT);
	gdt_set_gate(GDT_INDEX_USER_DS, USER_DS_FLAG, USER_DS_BASE, USER_DS_LIMIT);
	gdt_set_gate(GDT_INDEX_TSS, TSS_FLAG, TSS_BASE, TSS_LIMIT);
	gdt_set_gate(GDT_INDEX_LDT, LDT_FLAG, LDT_BASE, LDT_LIMIT);
	
	gdt_loader((u32)&gdtr_reg);
	logging("init GDT success!\n");
};

void idt_set_gate(u8 num, u32 base, u16 selector, u8 flags)
{
	idt_entry[num].base_low = base & 0xFFFF;
	idt_entry[num].base_high = (base >> 16) & 0xFFFF;

	idt_entry[num].selector = selector;
	idt_entry[num].always0 = 0;

	idt_entry[num].flags= flags;
};

/*中断描述符表的初始化*/
void init_idt()
{

	// 重新映射 IRQ 表
	// 两片级联的 Intel 8259A 芯片
	// 主片端口 0x20 0x21
	// 从片端口 0xA0 0xA1

	// 初始化主片、从片
	// 0001 0001
	outb(0x20, 0x11);
	outb(0xA0, 0x11);

	// 设置主片 IRQ 从 0x20(32) 号中断开始
	outb(0x21, 0x20);

	// 设置从片 IRQ 从 0x28(40) 号中断开始
	outb(0xA1, 0x28);

	// 设置主片 IR2 引脚连接从片
	outb(0x21, 0x04);

	// 告诉从片输出引脚和主片 IR2 号相连
	outb(0xA1, 0x02);

	// 设置主片和从片按照 8086 的方式工作
	outb(0x21, 0x01);
	outb(0xA1, 0x01);

	// 设置主从片允许中断
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

	/*idtr的初始化*/
	struct idtr_t idtr_reg = {0};
	idtr_reg.length = sizeof(gate_desc_t)*IDT_ENTRY_LEN -1;
	idtr_reg.base = (u32)&idt_entry;
	
	/*中断描述符表的定义及初始化*/
	int i;

	set_trap_gate(0,&divide_error);
	set_trap_gate(1,&debug);
	set_trap_gate(2,&nmi);
	set_system_gate(3,&int3);   /* int3-5 can be called from all */
	set_system_gate(4,&overflow);
	set_system_gate(5,&bounds);
	set_trap_gate(6,&invalid_op);
	set_trap_gate(7,&device_not_available);
	set_trap_gate(8,&double_fault);
	set_trap_gate(9,&coprocessor_segment_overrun);
	set_trap_gate(10,&invalid_TSS);
	set_trap_gate(11,&segment_not_present);
	set_trap_gate(12,&stack_segment);
	set_trap_gate(13,&general_protection);
	set_trap_gate(14,&page_fault);
	set_trap_gate(15,&reserved);
	set_trap_gate(16,&coprocessor_error);
	// 下面将int17-48 的陷阱门先均设置为reserved，以后每个硬件初始化时会重新设置自己的陷阱门。
	for (i=17;i<48;i++)
	{
		set_trap_gate(i,&reserved);
	}

	set_trap_gate(45,&irq13);               // 设置协处理器的陷阱门。
	outb_p(inb_p(0x21)&0xfb,0x21);          // 允许主8259A 芯片的IRQ2 中断请求。
	outb_p(inb_p(0xA1)&0xdf,0xA1);            // 允许从8259A 芯片的IRQ13 中断请求。
	set_trap_gate(39,&parallel_interrupt);  // 设置并行口的陷阱门。


	asm volatile("lidtl %0" : : "m" (idtr_reg));
};
