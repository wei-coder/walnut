/*
filename:	console.h
author:	wei-coder
date:	2017-12
purpose:	IO操作的头文件，内联函数定义
*/

#include "types.h"
#include "io.h"

#define HEX	16
#define DEC	10
#define OCT	8

typedef enum color {
	black = 0,				//黑
	blue = 1,				//蓝
	green = 2,				//绿
	cyan = 3,				//青
	red = 4,					//红
	magenta = 5,			//品红
	brown = 6,				//棕
	light_grey = 7,			//浅灰
	dark_grey = 8,			//深灰
	light_blue = 9,			//浅蓝
	light_green = 10,			//浅绿
	light_cyan = 11,			//浅青
	light_red = 12,			//浅红
	light_magenta = 13,		//浅品红
	light_brown = 14, 		//浅棕(黄)
	white = 15				//白
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


//VGA显存起始地址
static u16 * video_memory = (u16*)0xB8000;

//记录光标的坐标位置变量
static u8	horiz_c = 0;
static u8	verti_c = 0;


