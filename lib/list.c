/*
filename:	list.c
author:	wei-code
date:	2018-04
prupose:	链表操作的基本实现
*/

#include "list.h"
#include "heap.h"

#if 0
list_t* create_list()
{
	list_t * plist = malloc();
	if(NULL != plist)
	{
		plist->head = NULL;
		plist->tail = NULL;
		return plist;
	}
	return NULL;
};

void insert(node_t* pNode, list_t* plist)
{
	if ((NULL == pNode) || (NULL == plist))
	{
		return;
	};
	
	if (NULL == plist->head)
	{
		pNode->next = NULL;
		pNode->prev = NULL;
		plist->head = pNode;
		plist->tail = pNode;
	}
	else
	{
		pNode->next = NULL;
		pNode->prev = plist->tail;
		plist->tail->next = pNode;
		plist->tail = pNode;
	};

};

bool delete(node_t* pNode, list_t* plist)
{
	bool ret = FALSE;
	if (NULL == plist)
	{
		return ret;
	};
	
	node_t* tmpNode = plist->head;
	while (NULL != tmpNode)
	{
		if(pNode != tmpNode)
		{
			tmpNode = tmpNode->next;
			continue;
		};
		if(NULL != tmpNode->prev)
		{
			tmpNode->prev->next = tmpNode->next;
		}
		if(NULL != tmpNode->next)
		{
			tmpNode->next->prev = tmpNode->prev;
		}
		ret = free(tmpNode);
		break;
	};
	return ret;
};

void destroy(list_t* plist)
{
	if (NULL == plist)
	{
		return;
	};
	while (NULL != plist->head)
	{
		pop(plist);
	};
};

void pop(list_t* plist)
{
	node_t* tmpNode = plist->head;
	plist->head = plist->head->next;
	if (NULL != plist->head)
	{
		plist->head->prev = NULL;
	};
	free(tmpNode);
};
#endif
