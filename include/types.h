/*
filename: types.h
author:	wei-coder
date:	2017-12
purpose:	内核的全局类型定义
*/

#ifndef INCLUDE_TYPES_H_
#define INCLUDE_TYPES_H_

#ifndef NULL
	#define NULL 0
#endif

#ifndef TRUE
	#define TRUE  1
	#define FALSE 0
#endif


//u~unsigned; s~signed
typedef unsigned int		u32;
typedef int				s32;
typedef unsigned short		u16;
typedef short				s16;
typedef unsigned char		u8;
typedef char				s8;

#endif
