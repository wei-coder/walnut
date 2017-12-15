/*
filename:	kernel.ld
author:	wei-coder
date:	2017-12
purpose:	内核编译的链接器脚本
*/

ENTRY(start)
SECTIONS
{
	. = 0x100000;

	.text :
	{
		*(.text)
		. = ALIGN(4096);
	}
	.data :
	{
		*(.data)
		*(.rodata)
		. = ALIGN(4096);
	}
	.bss :
	{
		*(.bss)
		. = ALIGN(4096);
	}
	.stab :
	{
		*(.stab)
		. = ALIGN(4096);
	}
	.stabstr :
	{
		*(.stabstr)
	 	. = ALIGN(4096);
	}
	
	/DISCARD/ : { *(.comment) *(.eh_frame) }
}

