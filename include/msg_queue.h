/*
filename:	msg_queue.h
author:	wei-code
date:	2018-04
prupose:	��Ϣ���е���ؽṹ����
*/

#ifndef __LIST_H
#define __LIST_H

#include "types.h"

typedef struct MSG
{
	u32 pid;
	u32 msg_type;
	u8 message[40];
}msg_t;

#endif


