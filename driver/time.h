/*
filename: time.c
author:wei-coder
time:  2018-11-21
purpose: 系统时钟相关函数声明及数据结构定义
*/
#ifndef _TIME_H__
#define _TIME_H__

#include <types.h>

#define _TIME_SEC	0x00
#define _TIME_MIN	0x02
#define _TIME_HOUR	0x04
#define _TIME_DAY	0x07
#define _TIME_MON	0x08
#define _TIME_YEAR	0x09

struct tm
{
	u32 tm_sec;
	u32 tm_min;
	u32 tm_hour;
	u32 tm_day;
	u32 tm_mon;
	u32 tm_year;
	u32	tm_wday;
	u32	tm_yday;
};

time_t mktime(struct tm * t);

void localtime(struct tm * _tm, time_t _time);

int sys_time();

#endif
