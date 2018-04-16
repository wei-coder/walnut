/*
filename:	list.h
author:	wei-code
date:	2018-04
prupose:	链表的相关数据结构定义及函数声明
*/

#ifndef __LIST_H
#define __LIST_H

#include "types.h"

typedef struct NODE
{
	void* pNode;
	struct NODE * next;
	struct NODE * prev;
}node_t;

typedef struct LIST
{
	node_t* head;
	node_t* tail;
}list_t;

list_t* create_list();
void insert(node_t* pNode, list_t* plist);
bool delete(node_t* pNode, list_t* plist);
void pop(list_t* plist);
void destroy(list_t* plist);
#endif

