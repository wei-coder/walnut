/*
filename:	console.h
author:	wei-coder
date:	2017-12
purpose:	IO������ͷ�ļ���������������
*/

#include "types.h"
#include "io.h"

#define HEX	16
#define DEC	10
#define OCT	8

typedef enum color {
	black = 0,				//��
	blue = 1,				//��
	green = 2,				//��
	cyan = 3,				//��
	red = 4,					//��
	magenta = 5,			//Ʒ��
	brown = 6,				//��
	light_grey = 7,			//ǳ��
	dark_grey = 8,			//���
	light_blue = 9,			//ǳ��
	light_green = 10,			//ǳ��
	light_cyan = 11,			//ǳ��
	light_red = 12,			//ǳ��
	light_magenta = 13,		//ǳƷ��
	light_brown = 14, 		//ǳ��(��)
	white = 15				//��
} color_t;

void clear_screen();
static void scroll();
static void move_cursor();
void show_char(char c);
void show_char_color(char c, color_t back, color_t fore);
void show_string(char* string);
void show_string_color(char* string, color_t back, color_t fore);
void show_hex_num(u32 num);
void show_hex_num_color(u32 num, color_t back, color_t fore);
void show_dec_num(u32 num);
void show_dec_num_color(u32 num, color_t back, color_t fore);


//VGA�Դ���ʼ��ַ
static u16 * video_memory = (u16*)0xB8000;

//��¼��������λ�ñ���
static u8	horiz_c = 0;
static u8	verti_c = 0;

