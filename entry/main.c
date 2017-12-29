/*
filename: main.c
author:	wei-code
date:	2017-12
prupose:	操作系统的入口函数
*/
#include "types.h"
#include "console.h"
#include "pm.h"
#include  "timer.h"
#include "memory.h"
#include "multiboot.h"

// 开启分页机制之后的内核栈
char kern_stack[STACK_SIZE]  __attribute__ ((aligned(16)));

// 内核栈的栈顶
u32 kern_stack_top;

extern multiboot_t *glb_mboot_ptr;

void	entry_kernel();


/*内核使用的临时页目录表和页表
该地址必须是页对齐的，0~640K肯定是空的
临时页目录表只有一项，及一个有效页表，该页表1024个表项
指向1024个物理页，即1M空间*/
__attribute__((section(".init.data"))) u32 *pgd_tmp = (u32 *)0x1000;
__attribute__((section(".init.data"))) u32 *pte_low = (u32 *)0x2000;
__attribute__((section(".init.data"))) u32 *pte_hign = (u32 *)0x3000;

__attribute__((section(".init.text"))) int main()
{
	pgd_tmp[0] = (u32)pte_low | PDT_FLAG;
	pgd_tmp[PAGE_INDEX(PAGE_OFFSET)] =  (u32)pte_hign | PDT_FLAG;

	 u16* v_memory = (u16*)0xB8000;
	 u8	blank_attr = (0 << 4) | (15 & 0x0F);
	u16	blank = 0x20 | (blank_attr << 8);

	int i = 0;
	for (i = 0; i < 80 * 25; i++)
	{
		v_memory[i] = blank;
	};


	//映射内核虚拟地址4MB到物理地址的4MB
	for(i = 0; i<PTE_LEN; i++)
	{
		pte_low[i] = ((i << 12) | PAGE_FLAG);
	}

	// 映射 0x00000000-0x00400000 的物理地址到虚拟地址 0xC0000000-0xC0400000
	for(i=0; i<1024; i++)
	{
		pte_hign[i] = (i << 12) | PAGE_FLAG;
	}

	// 设置临时页表
	asm volatile ("mov %0, %%cr3" : : "r" (pgd_tmp));

	u32 cr0;

	// 启用分页，将 cr0 寄存器的分页位置为 1 就好
	asm volatile ("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0" : : "r" (cr0));
	
	// 切换内核栈
	kern_stack_top = ((u32)kern_stack + STACK_SIZE);
	asm volatile ("mov %0, %%esp\n\t"
			"xor %%ebp, %%ebp" : : "r" (kern_stack_top));

	// 更新全局 multiboot_t 指针
	glb_mboot_ptr =  mboot_ptr_tmp + PAGE_OFFSET;

	// 调用内核初始化函数
	entry_kernel();
	return 0;
};

void	entry_kernel()
{
	//while(1);
	char string[] = "hello walnut os!\n";
	clear_screen();
	
	show_string_color(string, 0, 4);
	
	//while(1);

	init_gdt();
	init_idt();

	printf("kernel in memory start: 0x%08X\n", kern_start);
	printf("kernel in memory end: 0x%08X\n", kern_end);
	printf("kernel in memory used: %d KB\n\n", (kern_end - kern_start + 1023) / 1024);

	show_mem_map();
	
	init_pmm();
	init_vmm();

	//while(1);

	/*开中断*/
	asm volatile ("sti");
	
}

