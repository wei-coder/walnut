/*
filename:	heap.h
author:		wei-coder
date:		2017-12
purpose:	���ڴ�������غ��������ͱ�������
*/

#ifndef __HEAP_H
#define __HEAP_H

#include "types.h"
#include "memory.h"

// ����ʼ��ַ
#define HEAP_START 0xE0000000

/*����ṹ����*/
typedef struct NODE
{
	u32 allocated:1;			//�ڴ���Ƿ��ѷ���
	u32	len:31;				//�ڴ��ĳ��ȣ�����������ṹ�ĳ��ȣ������������2G
	struct NODE* next;
	struct NODE* prev;
}node_t;

typedef struct LIST
{
	node_t * head;
	node_t * tail;
}list_t;

// �ڴ�����ṹ
typedef struct header {
	struct header *prev; 	// ǰ���ڴ�����ṹָ��
	struct header *next;
	u32 allocated : 1;	// ���ڴ���Ƿ��Ѿ�������
	u32 length : 31; 	// ��ǰ�ڴ��ĳ���
} header_t;

extern pdt_t* pdt;

// ��ʼ����
void init_heap();

// �ڴ�����
void *kmalloc(u32 len);

// �ڴ��ͷ�
void kfree(void *p);

// �����ں˶������ͷ�
void test_heap();

#endif
