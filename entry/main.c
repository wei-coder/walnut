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
char user_stack[STACK_SIZE]  __attribute__ ((aligned(16)));

// �ں�ջ��ջ��
u32 user_stack_top;

void	entry_kernel();


/*�ں�ʹ�õ���ʱҳĿ¼���ҳ��
�õ�ַ������ҳ����ģ�0~640K�϶��ǿյ�
��ʱҳĿ¼��ֻ��һ���һ����Чҳ����ҳ��1024������
ָ��1024������ҳ����4M�ռ�*/
__attribute__((section(".init.data"))) pdt_t *pdt_tmp  = (pdt_t *)0x1000;
__attribute__((section(".init.data"))) pdt_t *pte_low  = (pdt_t *)0x2000;
__attribute__((section(".init.data"))) pdt_t *pte_hign = (pdt_t *)0x3000;

__attribute__((section(".init.text"))) int main()
{
	pdt_tmp[0] = (u32)(pte_low) | PDT_FLAG;
	pdt_tmp[PDT_INDEX(PAGE_OFFSET)] =  (u32)(pte_hign) | PDT_FLAG;

	int i = 0;

	// ӳ�� 0x00000000-0x00400000 �������ַ�������ַ 0x00000000-0x00400000
	for(i = 0; i<PTE_LEN; i++)
	{
		pte_low[i] = ((i << 12) | PAGE_FLAG);
	}

	// ӳ�� 0x00000000-0x00400000 �������ַ�������ַ 0xC0000000-0xC0400000
	for(i=0; i<PTE_LEN; i++)
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
	user_stack_top = ((u32)user_stack + STACK_SIZE);
	asm volatile ("mov %0, %%esp\n\t"
			"xor %%ebp, %%ebp" : : "r" (user_stack_top));

	// ����ȫ�� multiboot_t ָ��
	glb_mboot_ptr =  mboot_ptr_tmp + PAGE_OFFSET;

	//printf("run in low memory! pte_high = 0x%08x\n", (u32)pte_hign);

	// �����ں˳�ʼ������
	entry_kernel();
	return 0;
};

void entry_kernel()
{
	char string[] = "welcome to walnut os world!\n";
	clear_screen();
	show_string_color(string, 0, 4);
	init_debug();
	init_gdt();
	init_idt();

	init_pmm();
	init_vmm();
	init_heap();
	init_timer(100);

	sched_init();
	/*���ж�*/
	asm volatile ("sti");

	move_to_user_mode();
	fork();
	while(1)
	{
		init();         // ���½����ӽ��̣�����1����ִ�С�
	}
}

