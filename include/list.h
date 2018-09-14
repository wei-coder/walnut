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

struct list_head
{
	struct list_head * next;
	struct list_head * prev;
};


typedef struct list_node
{
	void* pNode;
	struct list_node * next;
	struct list_node * prev;
}lnode_t;

typedef struct slist_node
{
	void* pNode;
	struct slist_node * next;
}snode_t;

typedef struct dlist
{
	lnode_t * head;
	bool (*isequal)(void *,void*);
	bool (*release)(void *);
	bool (*insert)(void *, void *);
	void (*del)(void *);
	void (*destroy)();
	bool (*search)(lnode_t *, void *);
}dlist_t;

//元素顺序不要调整，否则完蛋
typedef struct slist
{
	snode_t * head;
	bool (*isequal)(void *,void*);
	bool (*release)(void *);
	bool (*insert)(void *, void *);
	void (*del)(void *);
	void (*destroy)();
	bool (*search)(snode_t *, void *);
}slist_t;

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

