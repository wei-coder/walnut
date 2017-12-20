/*
filename:	pm.c
author:	wei-coder
date:	2017-12
purpose:	ϵͳ���뱣��ģʽ����Ҫ�ĸ����
*/
#include "pm.h"
#include "io.h"
#include "string.h"
#include "console.h"

/*ȫ����������Ķ���*/
static glb_desc_t gdt_entry[GDT_ENTRY_LEN] = {0};

/*�жϴ�������Ķ���*/
int_handler_ptr isr_entry[IDT_ENTRY_LEN] = {0};

/*�ж���������Ķ��弰��ʼ��*/
static gate_desc_t idt_entry[IDT_ENTRY_LEN] = {0};

static void gdt_set_gate(u8 num, u16 flags, u32 base, u32 limit)
{
	gdt_entry[num].desc_low = ((limit) & 0xffff) | (((base) & 0xffff) << 16);
	gdt_entry[num].desc_high = ((base) & 0xff)| (((flags) & 0xf0ff) << 8) | \
			((limit) & 0xf0000) | ((base) & 0xff000000);
};


/*ȫ����������ĳ�ʼ��*/
void init_gdt()
{
	struct gdtr_t gdtr_reg = {0};
	gdtr_reg.length = sizeof(glb_desc_t)*GDT_ENTRY_LEN - 1;
	gdtr_reg.base_addr = (u32)&gdt_entry;
	
	/*ȫ����������Ķ���*/
	memset(gdt_entry, 0, 8*GDT_ENTRY_LEN);
	gdt_set_gate(GDT_ENTRY_NULL,0, 0, 0);
	gdt_set_gate(GDT_ENTRY_KERNEL_CS, KERNEL_CS_FLAG, KERNEL_CS_BASE, KERNEL_CS_LIMIT);
	gdt_set_gate(GDT_ENTRY_KERNEL_DS, KERNEL_DS_FLAG, KERNEL_DS_BASE, KERNEL_DS_LIMIT);
	gdt_set_gate(GDT_ENTRY_USER_CS, USER_CS_FLAG, USER_CS_BASE, USER_CS_LIMIT);
	gdt_set_gate(GDT_ENTRY_USER_DS, USER_DS_FLAG, USER_DS_BASE, USER_DS_LIMIT);
	
	gdt_loader((u32)&gdtr_reg);
};

static void idt_set_gate(u8 num, u32 base, u16 selector, u8 flags)
{
	idt_entry[num].base_low = base & 0xFFFF;
	idt_entry[num].base_high = (base >> 16) & 0xFFFF;

	idt_entry[num].selector = selector;
	idt_entry[num].always0 = 0;

	idt_entry[num].flags= flags;
};

/*�ж���������ĳ�ʼ��*/
void init_idt()
{

	memset(isr_entry, 0, 4*IDT_ENTRY_LEN);
	memset(idt_entry, 0, 8*IDT_ENTRY_LEN);

	// ����ӳ�� IRQ ��
	// ��Ƭ������ Intel 8259A оƬ
	// ��Ƭ�˿� 0x20 0x21
	// ��Ƭ�˿� 0xA0 0xA1

	// ��ʼ����Ƭ����Ƭ
	// 0001 0001
	outb(0x20, 0x11);
	outb(0xA0, 0x11);

	// ������Ƭ IRQ �� 0x20(32) ���жϿ�ʼ
	outb(0x21, 0x20);

	// ���ô�Ƭ IRQ �� 0x28(40) ���жϿ�ʼ
	outb(0xA1, 0x28);

	// ������Ƭ IR2 �������Ӵ�Ƭ
	outb(0x21, 0x04);

	// ���ߴ�Ƭ������ź���Ƭ IR2 ������
	outb(0xA1, 0x02);

	// ������Ƭ�ʹ�Ƭ���� 8086 �ķ�ʽ����
	outb(0x21, 0x01);
	outb(0xA1, 0x01);

	// ��������Ƭ�����ж�
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

	/*idtr�ĳ�ʼ��*/
	struct idtr_t idtr_reg = {0};
	idtr_reg.length = sizeof(glb_desc_t)*IDT_ENTRY_LEN -1;
	idtr_reg.base = (u32)&idt_entry;
	
	/*�ж���������Ķ��弰��ʼ��*/
	idt_set_gate(0,(u32)isr0, 0x08, 0x8E);
	idt_set_gate(1,(u32)isr1, 0x08, 0x8E);
	idt_set_gate(2,(u32)isr2, 0x08, 0x8E);
	idt_set_gate(3,(u32)isr3, 0x08, 0x8E);
	idt_set_gate(4,(u32)isr4, 0x08, 0x8E);
	idt_set_gate(5,(u32)isr5, 0x08, 0x8E);
	idt_set_gate(6,(u32)isr6, 0x08, 0x8E);
	idt_set_gate(7,(u32)isr7, 0x08, 0x8E);
	idt_set_gate(8,(u32)isr8, 0x08, 0x8E);
	idt_set_gate(9,(u32)isr9, 0x08, 0x8E);
	idt_set_gate(10,(u32)isr10, 0x08, 0x8E);
	idt_set_gate(11,(u32)isr11, 0x08, 0x8E);
	idt_set_gate(12,(u32)isr12, 0x08, 0x8E);
	idt_set_gate(13,(u32)isr13, 0x08, 0x8E);
	idt_set_gate(14,(u32)isr14, 0x08, 0x8E);
	idt_set_gate(15,(u32)isr15, 0x08, 0x8E);
	idt_set_gate(16,(u32)isr16, 0x08, 0x8E);
	idt_set_gate(17,(u32)isr17, 0x08, 0x8E);
	idt_set_gate(18,(u32)isr18, 0x08, 0x8E);
	idt_set_gate(19,(u32)isr19, 0x08, 0x8E);
	idt_set_gate(20,(u32)isr20, 0x08, 0x8E);
	idt_set_gate(21,(u32)isr21, 0x08, 0x8E);
	idt_set_gate(22,(u32)isr22, 0x08, 0x8E);
	idt_set_gate(23,(u32)isr23, 0x08, 0x8E);
	idt_set_gate(24,(u32)isr24, 0x08, 0x8E);
	idt_set_gate(25,(u32)isr25, 0x08, 0x8E);
	idt_set_gate(26,(u32)isr26, 0x08, 0x8E);
	idt_set_gate(27,(u32)isr27, 0x08, 0x8E);
	idt_set_gate(28,(u32)isr28, 0x08, 0x8E);
	idt_set_gate(29,(u32)isr29, 0x08, 0x8E);
	idt_set_gate(30,(u32)isr30, 0x08, 0x8E);
	idt_set_gate(31,(u32)isr31, 0x08, 0x8E);

	idt_set_gate(32,(u32)irq0, 0x08, 0x8E);
	idt_set_gate(33,(u32)irq1, 0x08, 0x8E);
	idt_set_gate(34,(u32)irq2, 0x08, 0x8E);
	idt_set_gate(35,(u32)irq3, 0x08, 0x8E);
	idt_set_gate(36,(u32)irq4, 0x08, 0x8E);
	idt_set_gate(37,(u32)irq5, 0x08, 0x8E);
	idt_set_gate(38,(u32)irq6, 0x08, 0x8E);
	idt_set_gate(39,(u32)irq7, 0x08, 0x8E);
	idt_set_gate(40,(u32)irq8, 0x08, 0x8E);
	idt_set_gate(41,(u32)irq9, 0x08, 0x8E);
	idt_set_gate(42,(u32)irq10, 0x08, 0x8E);
	idt_set_gate(43,(u32)irq11, 0x08, 0x8E);
	idt_set_gate(44,(u32)irq12, 0x08, 0x8E);
	idt_set_gate(45,(u32)irq13, 0x08, 0x8E);
	idt_set_gate(46,(u32)irq14, 0x08, 0x8E);
	idt_set_gate(47,(u32)irq15, 0x08, 0x8E);

	idt_set_gate(255,(u32)isr255, 0x08, 0x8E);

	asm volatile("lidtl %0" : : "m" (idtr_reg));
};

/*ע��һ���жϴ�����*/
void register_int_handler(u8 num, int_handler_ptr func)
{
	isr_entry[num] = func;
};

/*�����жϴ�����*/
void isr_handler(int_cont_t * context)
{
	if (isr_entry[context->int_no])
	{
		isr_entry[context->int_no](context);
	}
	else
	{
		show_string("there is not this ISR!\n");
	}
};

/*IRQ ������*/
void irq_handler(int_cont_t *context)
{
	// �����жϽ����źŸ� PICs
	// �������ǵ����ã��� 32 ���ж���Ϊ�û��Զ����ж�
	// ��Ϊ��Ƭ�� Intel 8259A оƬֻ�ܴ��� 8 ���ж�
	// �ʴ��ڵ��� 40 ���жϺ����ɴ�Ƭ�����
	if (context->int_no >= 40)
	{
		// ���������źŸ���Ƭ
		outb(0xA0, 0x20);
	}

	// ���������źŸ���Ƭ
	outb(0x20, 0x20);

	if (isr_entry[context->int_no])
	{
		isr_entry[context->int_no](context);
	}
	else
	{
		show_string("there is not this interrupt handle!\n");
	}
};

