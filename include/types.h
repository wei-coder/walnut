/*
filename: types.h
author:	wei-coder
date:	2017-12
purpose:	内核的全局类型定义
*/

#pragma once

#ifndef NULL
	#define NULL 0
#endif

#ifndef TRUE
	#define TRUE  1
	#define FALSE 0
#endif

#ifndef KER_SUC
	#define KER_SUC 0
#endif

#ifndef KER_FAIL
	#define KER_FAIL -1
#endif


typedef long ptrdiff_t;

//u~unsigned; s~signed
typedef unsigned long		ulong;
typedef unsigned int		u32;
typedef int					s32;
typedef unsigned short		u16;
typedef short				s16;
typedef unsigned char		u8;
typedef char				s8;
typedef unsigned int		size_t;
typedef int 				pid_t;		
typedef unsigned short 		uid_t;
typedef unsigned char 		gid_t;
typedef unsigned short 		dev_t;
typedef unsigned short 		ino_t;
typedef unsigned short 		mode_t;
typedef unsigned short 		umode_t;
typedef unsigned char 		nlink_t;
typedef int 				daddr_t;
typedef unsigned long		off_t;
typedef unsigned long 		time_t;

typedef enum
{
	false	= 0,
	true	= 1
}bool;

static inline int isdigit(int ch)
{
	return (ch >= '0') && (ch <= '9');
}

static inline int isxdigit(int ch)
{
	if (isdigit(ch))
		return TRUE;

	if ((ch >= 'a') && (ch <= 'f'))
		return TRUE;

	return (ch >= 'A') && (ch <= 'F');
}
