/*
filename: main.c
author:	wei-code
date:	2017-12
prupose:	操作系统的入口函数
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

// 开启分页机制之后的内核栈
char user_stack[STACK_SIZE]  __attribute__ ((aligned(16)));

// 内核栈的栈顶
u32 user_stack_top;

void	entry_kernel();


/*内核使用的临时页目录表和页表
该地址必须是页对齐的，0~640K肯定是空的
临时页目录表只有一项，及一个有效页表，该页表1024个表项
指向1024个物理页，即4M空间*/
__attribute__((section(".init.data"))) pdt_t *pdt_tmp  = (pdt_t *)0x1000;
__attribute__((section(".init.data"))) pdt_t *pte_low  = (pdt_t *)0x2000;
__attribute__((section(".init.data"))) pdt_t *pte_hign = (pdt_t *)0x3000;

__attribute__((section(".init.text"))) int main()
{
	pdt_tmp[0] = (u32)(pte_low) | PDT_FLAG;
	pdt_tmp[PDT_INDEX(PAGE_OFFSET)] =  (u32)(pte_hign) | PDT_FLAG;

	int i = 0;

	// 映射 0x00000000-0x00400000 的物理地址到虚拟地址 0x00000000-0x00400000
	for(i = 0; i<PTE_LEN; i++)
	{
		pte_low[i] = ((i << 12) | PAGE_FLAG);
	}

	// 映射 0x00000000-0x00400000 的物理地址到虚拟地址 0xC0000000-0xC0400000
	for(i=0; i<PTE_LEN; i++)
	{
		pte_hign[i] = (i << 12) | PAGE_FLAG;
	}

	// 设置临时页表
	asm volatile ("mov %0, %%cr3" : : "r" (pdt_tmp));

	u32 cr0;

	// 启用分页，将 cr0 寄存器的分页位置为 1 就好
	asm volatile ("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0" : : "r" (cr0));
	
	// 切换内核栈
	user_stack_top = ((u32)user_stack + STACK_SIZE);
	asm volatile ("mov %0, %%esp\n\t"
			"xor %%ebp, %%ebp" : : "r" (user_stack_top));

	// 更新全局 multiboot_t 指针
	glb_mboot_ptr =  mboot_ptr_tmp + PAGE_OFFSET;

	//printf("run in low memory! pte_high = 0x%08x\n", (u32)pte_hign);

	// 调用内核初始化函数
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
	/*开中断*/
	asm volatile ("sti");

	move_to_user_mode();
	fork();
	while(1)
	{
		init();         // 在新建的子进程（任务1）中执行。
	}
}

