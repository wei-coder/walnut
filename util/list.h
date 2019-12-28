/*
filename:	list.h
author:	wei-code
date:	2018-04
prupose:	链表的相关数据结构定义及函数声明
*/

#ifndef __LIST_H
#define __LIST_H

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
	void (*push)(lnode_t *, lnode_t *);
	lnode_t* (*que_pop)(lnode_t **);
	lnode_t* (*stack_pop)(lnode_t **);
	void (*del)(void *);
	void (*destroy)();
	bool (*search)(lnode_t **, void *);
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
	bool (*search)(snode_t **, void *);
}slist_t;

#define init_ring_list_head(phead) {(phead)->next = (phead);(phead)->prev = (phead);}
#define init_line_list_head(phead) {(phead)->next = NULL;(phead)->prev = NULL;}

dlist_t* create_dlist(bool (*isequal)(void*,void*), bool (*release)(void *));
struct list_head * getlast_lh(struct list_head * plist);
void push_ring_list(struct list_head * phead, struct list_head * pNode);

#endif

