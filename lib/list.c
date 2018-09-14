/*
filename:	list.c
author:	wei-code
date:	2018-04
prupose:	链表操作的基本实现
*/

#include "list.h"

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

/*sigle list*/
bool insert_slist(void * fnode, void * new_node)
{
	slist_t * this = (slist_t*)((ulong)insert_slist - 3*(sizeof(ulong))); 

	 snode_t * tmpn = malloc(sizeof(snode_t));
	 if(NULL == tmpn)
	 {
	 	return false;
	 }
	 tmpn->pNode = new_node;
	 snode_t * tmpf = NULL;
	 if(this->search(&tmpf, fnode))
	 {
		tmpn->next = tmpf->next;
		tmpf->next = tmpn;
		return true;
	 }
	 return false;
}

void del_slist(void * node)
{
	snode_t * del_node = NULL;
	slist_t * this = (slist_t*)((ulong)del_slist - 4*(sizeof(ulong))); 
	snode_t * tmp = this->head;
	if(this->isequal(node,tmp->pNode))
	while(NULL != tmp->next)
	{
		if(this->isequal(node,tmp->next->pNode))
		{
			break;
		}
		tmp = tmp->next;
	}
	if(NULL == tmp)
	{
		return;
	}
	del_node = tmp->next;
	tmp->next = del_node->next;
	free(del_node);
	this->release(node);
}

void destroy_slist()
{
	slist_t * this = (slist_t*)((ulong)destroy_slist - 5*(sizeof(ulong))); 
	while(NULL != this->head)
	{
		snode_t * del_node = this->head;
		this->head = this->head->next;
		this->release(del_node->pNode);
		free(del_node);
	}
	free(this);
}

bool search_slist(snode_t ** rnode, void * pnode)
{
	slist_t * this = (slist_t*)((ulong)search_slist - 6*(sizeof(ulong))); 
	snode_t * tmp = this->head;
	while(NULL != tmp)
	{
	    if(this->isequal(tmp->pNode, pnode))
	    {
	    	*rnode = tmp;
	    	return true;
	    }
		tmp = tmp->next;
	}
	return false;
}

slist_t* create_slist(bool (*isequal)(void*,void*), void (*release)(void *))
{
	slist_t * plist = malloc(sizeof(slist_t));
	if(NULL != plist)
	{
		plist->head = malloc(sizeof(snode_t));
		if(NULL != plist->head)
		{
			plist->head->pNode = NULL;
			plist->head->next = NULL;
			plist->isequal = isequal;
			plist->release = release;
			plist->insert = insert_slist;
			plist->del = del_slist;
			plist->destroy = destroy_slist;
			plist->search = search_slist;
			return plist;
		}
		else
		{
			free(plist);
		}
	}
	return NULL;
}

/*double list*/
bool insert_dlist(void * fnode, void * new_node)
{
	dlist_t * this = (dlist_t*)((ulong)insert_dlist - 3*(sizeof(ulong))); 

	 lnode_t * tmpn = malloc(sizeof(lnode_t));
	 if(NULL == tmpn)
	 {
	 	return false;
	 }
	 tmpn->pNode = new_node;
	 lnode_t * tmpf = NULL;
	 if(this->search(&tmpf,fnode))
	 {
		 tmpn->next = tmpf->next;
		 tmpn->prev = tmpf;
		 tmpf->next = tmpn;
		 if(NULL != tmpf->next)
		 {
			tmpf->next->prev = tmpn;
		 }
		 return true;
	 }
	 return false;
}

void del_dlist(void * node)
{
	lnode_t * del_node = NULL;
	dlist_t * this = (dlist_t*)((ulong)del_slist - 4*(sizeof(ulong))); 
	lnode_t * tmp = this->head;
	if(this->isequal(node,tmp->pNode))
	while(NULL != tmp->next)
	{
		if(this->isequal(node,tmp->next->pNode))
		{
			break;
		}
		tmp = tmp->next;
	}
	if(NULL == tmp)
	{
		return;
	}
	del_node = tmp->next;
	tmp->next = del_node->next;
	if(NULL != del_node->next)
	{
		del_node->next->prev = tmp;
	}
	free(del_node);
	this->release(node);
}

void destroy_dlist()
{
	dlist_t * this = (dlist_t*)((ulong)destroy_slist - 5*(sizeof(ulong))); 
	while(NULL != this->head)
	{
		lnode_t * del_node = this->head;
		this->head = this->head->next;
		this->head->prev = NULL;
		this->release(del_node->pNode);
		free(del_node);
	}
	free(this);
}

bool search_dlist(lnode_t ** rnode, void * pnode)
{
	dlist_t * this = (dlist_t*)((ulong)search_slist - 6*(sizeof(ulong))); 
	lnode_t * tmp = this->head;
	while(NULL != tmp)
	{
	    if(this->isequal(tmp->pNode, pnode))
	    {
	    	*rnode = tmp;
	    	return true;
	    }
		tmp = tmp->next;
	}
	return false;
}

dlist_t* create_dlist(bool (*isequal)(void*,void*), void (*release)(void *))
{
	dlist_t * plist = malloc(sizeof(dlist_t));
	if(NULL != plist)
	{
		plist->head = malloc(sizeof(lnode_t));
		if(NULL != plist->head)
		{
			plist->head->pNode = NULL;
			plist->head->next = NULL;
			plist->head->prev = NULL;
			plist->isequal = isequal;
			plist->release = release;
			plist->insert = insert_dlist;
			plist->del = del_dlist;
			plist->destroy = destroy_dlist;
			plist->search = search_dlist;
			return plist;
		}
		else
		{
			free(plist);
		}
	}
	return NULL;
}

/*list_head*/
struct list_head * getlast_lh(struct list_head * plist)
{
	struct list_head * ptmp = plist;
	if(NULL == plist)
	{
		return NULL;
	}
	while(ptmp->next)
	{
		ptmp = ptmp->next;
	}
	return ptmp;
}

#endif
