/*
filename:	HD.h
author:	wei-code
date:	2018-04
prupose:	硬盘驱动的相关变量及函数声明
*/

#ifndef __HD_H
#define __HD_H

#include "types.h"

#define HD_PRI_DATA			0x1F0
#define HD_PRI_ERR_FEAT	0x1F1
#define HD_PRI_SEC_COUNT	0x1F2
#define HD_PRI_LBA_LOW		0x1F3
#define HD_PRI_LBA_MID		0x1F4
#define HD_PRI_LBA_HIGH	0x1F5
#define HD_PRI_DEVICE		0x1F6
#define HD_PRI_STAT_COM	0x1F7
#define HD_PRI_CONTROL		0x3F6

void init_hd();
#endif

