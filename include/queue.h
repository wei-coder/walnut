/*
filename:	list.h
author:	wei-code
date:	2018-04
prupose:	链表的相关数据结构定义及函数声明
*/

#ifndef __LIST_H
#define __LIST_H

#if 1
#include "types.h"

typedef struct list_node
{
	void* pNode;
	struct NODE * next;
	struct NODE * prev;
}lnode_t;

typedef struct queue
{
	lnode_t* head;
	lnode_t* tail;
}queue_t;

queue_t* create_queue();
void pop(queue_t* pque);
void push(queue_t* pque, lnode_t * pnode);
#endif
#endif

