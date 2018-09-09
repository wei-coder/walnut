/*
filename:	list.c
author:	wei-code
date:	2018-04
prupose:	链表操作的基本实现
*/

#include "queue.h"

#if 1
queue_t* create_queue()
{
	queue_t* pque = malloc();
	if(NULL != pque)
	{
		pque->head = NULL;
		pque->tail = NULL;
		return pque;
	}
	return NULL;
};

void pop(queue_t* pque)
{
	lnode_t* tmpNode = pque->head;
	pque->head = pque->head->next;
	if (NULL != pque->head)
	{
		pque->head->prev = NULL;
	};
	free(tmpNode);
};

void push(queue_t* pque, lnode_t* pnode)
{
	if ((NULL == pque) || (NULL == pnode))
	{
		return;
	};
	
	if (NULL == pque->tail)
	{
		pnode->next = NULL;
		pnode->prev = NULL;
		pque->head = pnode;
		pque->tail = pnode;
	}
	else
	{
		pnode->next = NULL;
		pnode->prev = pque->tail;
		pque->tail->next = pnode;
		pque->tail = pnode;
	};

};

#endif
