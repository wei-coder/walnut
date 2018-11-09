/*
filename:	list.h
author:	wei-code
date:	2018-04
prupose:	�����������ݽṹ���弰��������
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
	bool (*push)(void *);
	lnode_t* (*que_pop)(void *);
	lnode_t* (*stack_pop)(void *);
	void (*del)(void *);
	void (*destroy)();
	bool (*search)(lnode_t *, void *);
}dlist_t;

//Ԫ��˳��Ҫ�����������군
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

#endif
#endif

