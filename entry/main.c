/*
filename: main.c
author:	wei-code
date:	2017-12
prupose:	����ϵͳ����ں���
*/

#include "types.h"
#include "unistd.h"
#include "console.h"
#include "multiboot.h"
#include "memory.h"
#include "timer.h"
#include "kern_debug.h"
#include "heap.h"
#include "system.h"
#include "init.h"
#include "sched.h"
#include "trap_gate.h"

// ������ҳ����֮����ں�ջ
char kern_stack[STACK_SIZE]  __attribute__ ((aligned(16)));

// �ں�ջ��ջ��
u32 kern_stack_top;

void	entry_kernel();


/*�ں�ʹ�õ���ʱҳĿ¼���ҳ��
�õ�ַ������ҳ����ģ�0~640K�϶��ǿյ�
��ʱҳĿ¼��ֻ��һ���һ����Чҳ����ҳ��1024������
ָ��1024������ҳ����4M�ռ�*/
__attribute__((section(".init.data"))) u32 *pdt_tmp = (u32 *)0x1000;
__attribute__((section(".init.data"))) u32 *pte_low = (u32 *)0x2000;
__attribute__((section(".init.data"))) u32 *pte_hign = (u32 *)0x3000;

__attribute__((section(".init.text"))) int main()
{
	pdt_tmp[0] = (u32)pte_low | PDT_FLAG;
	pdt_tmp[PDT_INDEX(PAGE_OFFSET)] =  (u32)pte_hign | PDT_FLAG;

	int i = 0;

	// ӳ�� 0x00000000-0x00400000 �������ַ�������ַ 0x00000000-0x00400000
	for(i = 0; i<PTE_LEN; i++)
	{
		pte_low[i] = ((i << 12) | PAGE_FLAG);
	}

	// ӳ�� 0x00000000-0x00400000 �������ַ�������ַ 0xC0000000-0xC0400000
	for(i=0; i<1048; i++)
	{
		pte_hign[i] = (i << 12) | PAGE_FLAG;
	}

	// ������ʱҳ��
	asm volatile ("mov %0, %%cr3" : : "r" (pdt_tmp));

	u32 cr0;

	// ���÷�ҳ���� cr0 �Ĵ����ķ�ҳλ��Ϊ 1 �ͺ�
	asm volatile ("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0" : : "r" (cr0));
	
	// �л��ں�ջ
	kern_stack_top = ((u32)kern_stack + STACK_SIZE);
	asm volatile ("mov %0, %%esp\n\t"
			"xor %%ebp, %%ebp" : : "r" (kern_stack_top));

	// ����ȫ�� multiboot_t ָ��
	glb_mboot_ptr =  mboot_ptr_tmp + PAGE_OFFSET;

	// �����ں˳�ʼ������
	entry_kernel();
	return 0;
};

void entry_kernel()
{
	char string[] = "hello walnut os!\n";
	clear_screen();
	show_string_color(string, 0, 4);

	init_debug();
	init_gdt();
	init_idt();

	printf("kernel in memory start: 0x%08X\n", kern_start);
	printf("kernel in memory end: 0x%08X\n", kern_end);
	printf("kernel in memory used: %d KB\n\n", (kern_end - kern_start + 1023) / 1024);

	init_pmm();
	init_vmm();
	init_heap();
	trap_init();
	init_timer(1);

	//test_heap();
	
	sched_init();
	/*���ж�*/
	asm volatile ("sti");

	move_to_user_mode();
	while(1);
	printf("this is user mode\n");
	
	if (!fork()) 
	{
		init();         // ���½����ӽ��̣�����1����ִ�С�
	}


	
	//move_to_user_mode();
	
	while(1);
}

