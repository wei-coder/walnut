/*
filename:	heap.h
author:		wei-coder
date:		2017-12
purpose:	堆内存管理的相关函数声明和变量定义
*/

#include "types.h"

// 堆起始地址
#define HEAP_START 0xE0000000

/*链表结构定义*/
typedef struct NODE
{
	u32 allocated:1;
	u32	len:31;
	struct NODE* next;
	struct NODE* prev;
}node_t;

typedef struct LIST
{
	node_t * head;
	node_t * tail;
}list_t;

// 内存块管理结构
typedef struct header {
	struct header *prev; 	// 前后内存块管理结构指针
	struct header *next;
	u32 allocated : 1;	// 该内存块是否已经被申请
	u32 length : 31; 	// 当前内存块的长度
} header_t;

// 初始化堆
void init_heap();

// 内存申请
void *kmalloc(u32 len);

// 内存释放
void kfree(void *p);

// 测试内核堆申请释放
void test_heap();

