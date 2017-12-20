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
#include "pmm.h"

int main()
{
	char string[] = "hello walnut os!\n";
	clear_screen();
	show_string_color(string, 0, 4);

	init_gdt();
	init_idt();
	init_timer(200);

	/*开中断*/
	asm volatile ("sti");
	
	printf("kernel in memory start: 0x%08X\n", kern_start);
	printf("kernel in memory end: 0x%08X\n", kern_end);
	printf("kernel in memory used: %d KB\n\n", (kern_end - kern_start + 1023) / 1024);

	show_mem_map();
	return 0;
}

