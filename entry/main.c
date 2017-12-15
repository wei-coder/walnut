/*
filename: main.c
author:	wei-code
date:	2017-12
prupose:	操作系统的入口函数
*/
#include "types.h"
#include "console.h"

int main()
{
	char string[] = "hello walnut os!";

	clear_screen();
	show_string_color(string, 0, 4);
	
	return 0;
}

