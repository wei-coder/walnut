/*
filename: console.c
author:	wei-coder
date:	2017-12
purpose:	控制台操作函数
*/

#include "console.h"


/*
函数说明:  清屏函数
*/
void clear_screen()
{
	u8	blank_attr = (0 << 4) | (15 & 0x0F);
	u16	blank = 0x20 | (blank_attr << 8);

	for (int i = 0; i < 80 * 25; i++)
	{
		video_memory[i] = blank;
	}

	horiz_c = 0;
	verti_c = 0;
	move_cursor();
};

/*
函数说明:  移动光标
*/
static void move_cursor()
{
	// 屏幕是80 字节宽
	u16 cursorLocation = horiz_c* 80 + verti_c;

	//0x3D4和0X3D5两个端口可以用来读写显卡的内部寄存器
	//0x3D4用户写入要访问的寄存器编号，0x3D5用来读写寄存器数据
	//14, 15分别表示存放光标位置的两个寄存器，其中14接受高8bit，15接受低8bit
	outb(0x3D4, 14); 
	outb(0x3D5, cursorLocation >> 8); 		//发送位置的高8bit
	outb(0x3D4, 15); 
	outb(0x3D5, cursorLocation); 			//发送位置的低8bit
};

/*
函数说明: 滚屏
*/
static void scroll()
{
	u8 blank_attr = (0 << 4) | (15 & 0x0F);
	u16 blank = 0x20 | (blank_attr << 8);

	if(25 <= verti_c)
	{
		for(int i = 0; i < (24*80); i++)
		{
			video_memory[i] = video_memory[i+80];
		}

		for(int i = (24*80); i<(25*80); i++)
		{
			video_memory[i] = blank;
		}

		verti_c = 24;
		move_cursor();
	}
};

/*
函数说明:  显示字符，默认前白后黑
*/
void show_char(char c)
{
	show_char_color(c, black, white);
};

/*
函数说明:  显示字符，指定前后色
*/
void show_char_color(char c, color_t back, color_t fore)
{
	u8	char_attr = (back << 4) | (fore & 0x0F);
	u16	char_con = c | (char_attr << 8);

	//0x08是退格键
	//0x09是tab键
	if ((0x08 == c) && (horiz_c))
	{
		horiz_c--;
	}
	else if (0x09 == c)
	{
		/*右移1~8, 然后向左调整使得横坐标是8的倍数*/
		horiz_c = (horiz_c+8) & ~(8-1);			//~按位取反,&按位与
	}
	else if( '\r' == c)
	{
		/*回车*/
		horiz_c = 0;
	}
	else if ('\n' == c)
	{
		/*换行*/
		horiz_c = 0;
		verti_c ++;
	}
	else if ( ' '  <= c)
	{
		video_memory[verti_c*80 + horiz_c] = char_con ;
		horiz_c++;
	}
	
	if(80 < horiz_c)
	{
		horiz_c = 0;
		verti_c ++;
	}

	scroll();

};

/*
函数说明:  显示字符串，默认前白后黑
*/
void show_string(char* string)
{
	while(*string)
	{
		show_char(*string);
		string += 1;
	}
};

/*
函数说明:  显示字符串，指定前后色
*/
void show_string_color(char* string, color_t back, color_t fore)
{
	while(*string)
	{
		show_char_color(*string, back, fore);
		string += 1;
	}
};

/*
函数说明:数字转字符串,字符串空间至少11个字节
*/
char* itoa(u32 num, char* string, u8 radix)
{
	const char digit[] = "0123456789ABCDEF";
	char tmp[32] = {0};
	int i = 0;
	
	if(NULL == string)
	{
		return NULL;
	};

	if(0 == num)
	{
		tmp[0] = '0';
		i++;
	}
	else
	{
		while(0 != num)
		{
			num = num/radix;
			tmp[i++] = digit[num%HEX];
		}
	}
	
	if(HEX == radix)
	{
		*(string++) = '0';
		*(string++) = 'x';
	}
	else if(OCT == radix)
	{
		*(string++) = '0';
	}
	else if(DEC != radix)
	{
		return NULL;
	}
		
	
	while(i-- > 0)
	{
		*(string++) = tmp[i];
	}
	
	return string;
};

/*
函数说明:  显示16进制数，默认前白后黑
*/
void show_hex_num(u32 num)
{
	char string[11] = {0};
	
	if(NULL != itoa(num, string, HEX))
	{
		show_string( itoa(num, string, HEX));
	};
};

/*
函数说明:  显示16进制数，指定前后色
*/
void show_hex_num_color(u32 num, color_t back, color_t fore)
{
	char string[11] = {0};
	
	if(NULL != itoa(num, string, HEX))
	{
		show_string_color(string, back, fore);
	}
};

/*
函数说明:  显示10进制数，默认前白后黑
*/
void show_dec_num(u32 num)
{
	char string[11] = {0};
	
	if(NULL != itoa(num, string, DEC))
	{
		show_string(string);
	}
};

/*
函数说明:  显示10进制数，指定前后色
*/
void show_dec_num_color(u32 num, color_t back, color_t fore)
{
	char string[11] = {0};

	if(NULL != itoa(num, string, DEC))
	{
		show_string_color(string, back, fore);
	}
};



