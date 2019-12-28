/*
filename: console.c
author:	wei-coder
date:	2017-12
purpose:	控制台操作函数
*/

#include <kio.h>
#include "../mm/mm.h"
#include "kstring.h"
#include "console.h"

//VGA显存起始地址
static u16 * video_memory = (u16*)(0xB8000 + PAGE_OFFSET);

//记录光标的坐标位置变量
static u8	horiz_c = 0;
static u8	verti_c = 0;

/********************************************************
函数说明:  移动光标
********************************************************/
static void move_cursor()
{
	// 屏幕是80 字节宽
	u16 cursorLocation = horiz_c + verti_c* 80;

	//0x3D4和0X3D5两个端口可以用来读写显卡的内部寄存器
	//0x3D4用户写入要访问的寄存器编号，0x3D5用来读写寄存器数据
	//14, 15分别表示存放光标位置的两个寄存器，其中14接受高8bit，15接受低8bit
	outb_p(14, 0x3D4); 
	outb_p(cursorLocation >> 8, 0x3D5); 		//发送位置的高8bit
	outb_p(15, 0x3D4); 
	outb_p(cursorLocation, 0x3D5); 			//发送位置的低8bit
};


/********************************************************
函数说明: 滚屏
********************************************************/
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
	}
};

/********************************************************
函数说明:  清屏函数
********************************************************/
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

/********************************************************
函数说明:  显示字符，默认前白后黑
********************************************************/
void show_char(char c)
{
	show_char_color(c, black, white);
};

/********************************************************
函数说明:  显示字符，指定前后色
********************************************************/
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
	move_cursor();
};

/********************************************************
函数说明:  显示字符串，默认前白后黑
********************************************************/
void show_string(char* string)
{
	while(*string)
	{
		show_char(*string);
		string += 1;
	}
};

/********************************************************
函数说明:  显示字符串，指定前后色
********************************************************/
void show_string_color(char* string, color_t back, color_t fore)
{
	while(*string)
	{
		show_char_color(*string, back, fore);
		string += 1;
	}
};

/********************************************************
函数说明:数字转字符串,字符串空间至少11个字节
********************************************************/
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
			tmp[i++] = digit[num%radix];
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

/********************************************************
函数说明:  显示16进制数，默认前白后黑
********************************************************/
void show_hex_num(u32 num)
{
	char string[11] = {0};
	
	if(NULL != itoa(num, string, HEX))
	{
		show_string( itoa(num, string, HEX));
	};
};

/********************************************************
函数说明:  显示16进制数，指定前后色
********************************************************/
void show_hex_num_color(u32 num, color_t back, color_t fore)
{
	char string[11] = {0};
	
	if(NULL != itoa(num, string, HEX))
	{
		show_string_color(string, back, fore);
	}
};

/********************************************************
函数说明:  显示10进制数，默认前白后黑
********************************************************/
void show_dec_num(u32 num)
{
	char string[11] = {0};
	
	if(NULL != itoa(num, string, DEC))
	{
		show_string(string);
	}
};

/********************************************************
函数说明:  显示10进制数，指定前后色
********************************************************/
void show_dec_num_color(u32 num, color_t back, color_t fore)
{
	char string[11] = {0};

	if(NULL != itoa(num, string, DEC))
	{
		show_string_color(string, back, fore);
	}
};

static int skip_atoi(const char **s)
{
	int i = 0;

	while (isdigit(**s))
		i = i * 10 + *((*s)++) - '0';
	return i;
}

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SMALL	32		/* Must be 32 == 0x20 */
#define SPECIAL	64		/* 0x */

#define __do_div(n, base) ({ \
int __res; \
__res = ((unsigned long) n) % (unsigned) base; \
n = ((unsigned long) n) / (unsigned) base; \
__res; })

static char *number(char *str, long num, int base, int size, int precision,
		    int type)
{
	/* we are called with base 8, 10 or 16, only, thus don't need "G..."  */
	static const char digits[16] = "0123456789ABCDEF"; /* "GHIJKLMNOPQRSTUVWXYZ"; */

	char tmp[66];
	char c, sign, locase;
	int i;

	/* locase = 0 or 0x20. ORing digits or letters with 'locase'
	 * produces same digits or (maybe lowercased) letters */
	locase = (type & SMALL);
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 16)
		return NULL;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0)
		tmp[i++] = '0';
	else
		while (num != 0)
			tmp[i++] = (digits[__do_div(num, base)] | locase);
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type & (ZEROPAD + LEFT)))
		while (size-- > 0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;
	if (type & SPECIAL) {
		if (base == 8)
			*str++ = '0';
		else if (base == 16) {
			*str++ = '0';
			*str++ = ('X' | locase);
		}
	}
	if (!(type & LEFT))
		while (size-- > 0)
			*str++ = c;
	while (i < precision--)
		*str++ = '0';
	while (i-- > 0)
		*str++ = tmp[i];
	while (size-- > 0)
		*str++ = ' ';
	return str;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
	int len;
	unsigned long num;
	int i, base;
	char *str;
	const char *s;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */

	for (str = buf; *fmt; ++fmt) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}

		/* process flags */
		flags = 0;
	      repeat:
		++fmt;		/* this also skips first '%' */
		switch (*fmt) {
		case '-':
			flags |= LEFT;
			goto repeat;
		case '+':
			flags |= PLUS;
			goto repeat;
		case ' ':
			flags |= SPACE;
			goto repeat;
		case '#':
			flags |= SPECIAL;
			goto repeat;
		case '0':
			flags |= ZEROPAD;
			goto repeat;
		}

		/* get field width */
		field_width = -1;
		if (isdigit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (isdigit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
			qualifier = *fmt;
			++fmt;
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = ' ';
			*str++ = (unsigned char)va_arg(args, int);
			while (--field_width > 0)
				*str++ = ' ';
			continue;

		case 's':
			s = va_arg(args, char *);
			len = strnlen(s, precision);

			if (!(flags & LEFT))
				while (len < field_width--)
					*str++ = ' ';
			for (i = 0; i < len; ++i)
				*str++ = *s++;
			while (len < field_width--)
				*str++ = ' ';
			continue;

		case 'p':
			if (field_width == -1) {
				field_width = 2 * sizeof(void *);
				flags |= ZEROPAD;
			}
			str = number(str,
				     (unsigned long)va_arg(args, void *), 16,
				     field_width, precision, flags);
			continue;

		case 'n':
			if (qualifier == 'l') {
				long *ip = va_arg(args, long *);
				*ip = (str - buf);
			} else {
				int *ip = va_arg(args, int *);
				*ip = (str - buf);
			}
			continue;

		case '%':
			*str++ = '%';
			continue;

			/* integer number formats - set up the flags and "break" */
		case 'o':
			base = 8;
			break;

		case 'x':
			flags |= SMALL;
		case 'X':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			*str++ = '%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			continue;
		}
		if (qualifier == 'l')
			num = va_arg(args, unsigned long);
		else if (qualifier == 'h') {
			num = (unsigned short)va_arg(args, int);
			if (flags & SIGN)
				num = (short)num;
		} else if (flags & SIGN)
			num = va_arg(args, int);
		else
			num = va_arg(args, unsigned int);
		str = number(str, num, base, field_width, precision, flags);
	}
	*str = '\0';
	return str - buf;
}

int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsprintf(buf, fmt, args);
	va_end(args);
	return i;
}

int printf(const char *fmt, ...)
{
	char printf_buf[1024];
	va_list args;
	int printed;

	va_start(args, fmt);
	printed = vsprintf(printf_buf, fmt, args);
	va_end(args);

	show_string(printf_buf);

	return printed;
}


