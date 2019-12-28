/*
filename: time.c
author:wei-coder
time:  2018-11-21
purpose: 系统时钟相关函数实现
*/

#include <types.h>
#define __LIBRARY__
#include <unistd.h>
#include "time.h"
#include "io.h"

u8 mon_days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

_syscall1(time_t,time,time_t *,t_loc)

time_t mktime(struct tm * t)
{
	time_t now = 0;
	now += t->tm_sec;
	now += t->tm_min * 60;
	now += t->tm_hour * 3600;
	now += t->tm_day * 86400;
	for(int i=0; i<t->tm_mon; i++)
	{
		now += mon_days[i] * 86400;
	}
	if(((t->tm_year+1900)%4 == 0) && ((t->tm_year + 1900)%100 != 0))
	{
		now += 86400;
	}
	if(t->tm_year < 70)
	{
		return 0;
	}
	for(int i=1970; i<(t->tm_year+1900); i++)
	{
		if((i%4 == 0) && (i%100 != 0))
		{
			now += 366 * 86400;
		}
		else
		{
			now += 365 * 86400;
		}
	}
	return now;
}

void localtime(struct tm * _tm, time_t _time)
{
	int i = 0;
	time_t remain = _time;
	int second = 0;
	while((remain - second)>0)
	{
		remain -= second;
		if(((1970+i)%4 == 0)&&((1970+i)%100 != 0))
		{
			second = 366*86400;
		}
		else
		{
			second = 365*86400;
		}
		i++;
	}
	_tm->tm_year = 70+i-1;
	_tm->tm_yday = remain/86400;
	second = 0;
	i = 0;
	while((remain-second)>0)
	{
		remain -= second;
		second = mon_days[i] * 86400;
		if((1 == i)&&((1900+_tm->tm_year)%4 == 0)&&((1900+_tm->tm_year)%100 != 0))
		{
			second += 86400;
		}
		i++;
	}
	_tm->tm_mon = i-1;
	_tm->tm_day = remain/86400;
	remain %= 86400;
	_tm->tm_hour = remain/3600;
	remain %= 3600;
	_tm->tm_min = remain/60;
	remain %= 60;
	_tm->tm_sec = remain;
	_tm->tm_wday = (4 + _time%(7*86400))%7;
}

int sys_time()
{
	struct tm now = {0};
	
	u8 csec,cmin,chour,cday,cmon,cyear;
	
	outb_p(_TIME_SEC, 0x70); 
	csec = inb_p(0x71);
	now.tm_sec = (csec&0x0F) + 10*((csec&0xF0)>>4);
	
	outb_p(_TIME_MIN, 0x70); 
	cmin = inb_p(0x71);
	now.tm_min = (cmin&0x0F) + 10*((cmin&0xF0)>>4);
	
	outb_p(_TIME_HOUR, 0x70); 
	chour = inb_p(0x71);
	now.tm_hour = (chour&0x0F) + 10*((chour&0xF0)>>4);
	
	outb_p(_TIME_DAY, 0x70); 
	cday = inb_p(0x71);
	now.tm_day = (cday&0x0F) + 10*((cday&0xF0)>>4);
	
	outb_p(_TIME_MON, 0x70); 
	cmon = inb_p(0x71);
	now.tm_mon = (cmon&0x0F) + 10*((cmon&0xF0)>>4)-1;
	
	outb_p(_TIME_YEAR, 0x70); 
	cyear = inb_p(0x71);
	now.tm_mon = 100 + (cyear&0x0F) + 10*((cyear&0xF0)>>4);
	
	return (int)mktime(&now);
};

