/*
filename: main.c
author:	wei-code
date:	2017-12
prupose:	����ϵͳ����ں���
*/
#include "types.h"
#include "console.h"
#include "pm.h"

int main()
{
	char string[] = "hello walnut os!";

//	init_gdt();
//	init_idt();
		
	clear_screen();
	show_string_color(string, 0, 4);

//	asm volatile ("int $0x3");
//	asm volatile ("int $0x4");
	
	return 0;
}

