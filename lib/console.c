/*
filename: console.c
author:	wei-coder
date:	2017-12
purpose:	����̨��������
*/

#include "console.h"


/*
����˵��:  ��������
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
����˵��:  �ƶ����
*/
static void move_cursor()
{
	// ��Ļ��80 �ֽڿ�
	u16 cursorLocation = horiz_c* 80 + verti_c;

	//0x3D4��0X3D5�����˿ڿ���������д�Կ����ڲ��Ĵ���
	//0x3D4�û�д��Ҫ���ʵļĴ�����ţ�0x3D5������д�Ĵ�������
	//14, 15�ֱ��ʾ��Ź��λ�õ������Ĵ���������14���ܸ�8bit��15���ܵ�8bit
	outb(0x3D4, 14); 
	outb(0x3D5, cursorLocation >> 8); 		//����λ�õĸ�8bit
	outb(0x3D4, 15); 
	outb(0x3D5, cursorLocation); 			//����λ�õĵ�8bit
};

/*
����˵��: ����
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
����˵��:  ��ʾ�ַ���Ĭ��ǰ�׺��
*/
void show_char(char c)
{
	show_char_color(c, black, white);
};

/*
����˵��:  ��ʾ�ַ���ָ��ǰ��ɫ
*/
void show_char_color(char c, color_t back, color_t fore)
{
	u8	char_attr = (back << 4) | (fore & 0x0F);
	u16	char_con = c | (char_attr << 8);

	//0x08���˸��
	//0x09��tab��
	if ((0x08 == c) && (horiz_c))
	{
		horiz_c--;
	}
	else if (0x09 == c)
	{
		/*����1~8, Ȼ���������ʹ�ú�������8�ı���*/
		horiz_c = (horiz_c+8) & ~(8-1);			//~��λȡ��,&��λ��
	}
	else if( '\r' == c)
	{
		/*�س�*/
		horiz_c = 0;
	}
	else if ('\n' == c)
	{
		/*����*/
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
����˵��:  ��ʾ�ַ�����Ĭ��ǰ�׺��
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
����˵��:  ��ʾ�ַ�����ָ��ǰ��ɫ
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
����˵��:����ת�ַ���,�ַ����ռ�����11���ֽ�
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
����˵��:  ��ʾ16��������Ĭ��ǰ�׺��
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
����˵��:  ��ʾ16��������ָ��ǰ��ɫ
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
����˵��:  ��ʾ10��������Ĭ��ǰ�׺��
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
����˵��:  ��ʾ10��������ָ��ǰ��ɫ
*/
void show_dec_num_color(u32 num, color_t back, color_t fore)
{
	char string[11] = {0};

	if(NULL != itoa(num, string, DEC))
	{
		show_string_color(string, back, fore);
	}
};



