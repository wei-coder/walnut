/*
filename:	heap.c
author:		wei-coder
date:		2017-12
purpose:	���ڴ�������غ���ʵ��
*/

#include "heap.h"
#include "console.h"
#include "memory.h"

// �����ڴ��
static u32 alloc_chunk(u32 start, u32 len);

// �ͷ��ڴ��
static void free_block(node_t * block);

// �з��ڴ��
static void split_chunk(node_t *chunk, u32 len);

// �ϲ��ڴ��
static void merge_block(node_t * block);

static void init_node(node_t* pNode, u32 len);

static u32 heap_max = HEAP_START;

// �ڴ�����ͷָ��
static node_t* phead;

extern u32*	pdt;

void init_heap()
{
	phead = NULL;
}

void init_node(node_t* pNode, u32 len)
{
	if(NULL != pNode)
	{
		pNode->next = NULL;
		pNode->prev = NULL;
		pNode->allocated = 0;
		pNode->len = len;
	}
}

void* kmalloc(u32 len)
{
	u32 block_start = 0;
	len += sizeof(node_t);
	u32 chunk_size = 0;

	if(NULL == phead)
	{
		block_start = HEAP_START;
		phead = (node_t *)block_start;
		chunk_size = alloc_chunk(block_start, len);
		init_node(phead, chunk_size);
		//printf("phead=0x%08X\n", phead);
	}

	node_t* pNode = phead;
	while(pNode)
	{
		if (pNode->allocated == 0 && pNode->len >= len)
		{
			// ���յ�ǰ�����и��ڴ�
			split_chunk(pNode, len);
			pNode->allocated = 1;
			// ���ص�ʱ����뽫ָ��Ų������ṹ֮��
			return (void *)((u32)pNode + sizeof(node_t));
		}
		// �������ָ��
		if(pNode->next)
		{
			pNode = pNode->next;
		}
		else
		{
			break;
		}
	}

	block_start = (u32)pNode + pNode->len;
	chunk_size = alloc_chunk(block_start, len);
	//printf("block start=0x%08X; pNode");
	pNode->next = (node_t*)block_start;
	init_node(pNode->next,len);
	pNode->next->prev = pNode;

	return (void*)(block_start + sizeof(node_t));
	
}

/*
void *kmalloc(u32 len)
{
	// ����������ڴ泤�ȼ��Ϲ���ͷ�ĳ���
	// ��Ϊ���ڴ�������ͷŵ�ʱ��Ҫͨ���ýṹȥ����
	len += sizeof(node_t);

	node_t * p_node = phead;
	node_t * prev_node = NULL;

	while (p_node)
	{
		// �����ǰ�ڴ��û�б���������ҳ��ȴ��ڴ�����Ŀ�
		if (p_node->allocated == 0 && p_node->len >= len)
		{
			// ���յ�ǰ�����и��ڴ�
			split_chunk(p_node, len);
			p_node->allocated = 1;
			// ���ص�ʱ����뽫ָ��Ų������ṹ֮��
			return (void *)((u32)p_node + sizeof(node_t));
		}
		// �������ָ��
		prev_node = p_node;
		p_node = p_node->next;
		printf("prev_node addr = 0x%08X; p_node addr = 0x%08X \n", prev_node, p_node);
	}

	u32 chunk_start;

	// ��һ��ִ�иú������ʼ���ڴ����ʼλ��
	// ֮����ݵ�ǰָ���������ĳ��ȼ���
	if (prev_node)
	{
		chunk_start = (u32)prev_node + prev_node->len;
	}
	else
	{
		chunk_start = HEAP_START;
		mem_list.head = (node_t *)chunk_start;
		printf("block start addr=0x%08X\n", chunk_start);
	}
	
	// ����Ƿ���Ҫ�����ڴ�ҳ
	alloc_chunk(chunk_start, len);
	p_node = (node_t *)chunk_start;
	p_node->prev = prev_node;
	p_node->next = NULL;
	p_node->allocated = 1;
	p_node->len = len;
	
	if (prev_node)
	{
		prev_node->next = p_node;
	}

	return (void*)(chunk_start + sizeof(node_t));
}
*/

void kfree(void *p)
{
	// ָ����˵�����ṹ��������ʹ�ñ���� 0
	node_t *header = (node_t*)((u32)p - sizeof(node_t));
	header->allocated = 0;

	// �ϲ��ڴ��
	merge_block(header);
}

u32 alloc_chunk(u32 start, u32 len)
{
	u32 ret = 0;
	// �����ǰ�ѵ�λ���Ѿ���������������ڴ�ҳ
	// ����ѭ�������ڴ�ҳֱ���е��㹻�Ŀ����ڴ�
	while (start + len > heap_max)
	{
		u32 page = (u32)alloc_page();
		//printf("alloc chunk page = 0x%08X, start= 0x%08X\n", page, start);
		map((pdt_t*)pdt, heap_max, page, PAGE_FLAG);
		heap_max += PMM_PAGE_SIZE;
		ret += PMM_PAGE_SIZE;
	}
	return ret;
}

void free_block(node_t * block)
{
	if (block->prev == 0)
	{
		phead = NULL;
	}
	else
	{
		block->prev->next = 0;
	}

	// ���е��ڴ泬�� 1 ҳ�Ļ����ͷŵ�
	while ((heap_max - PMM_PAGE_SIZE) >= (u32)block)
	{
		heap_max -= PMM_PAGE_SIZE;
		u32 page;
		get_mapping((pdt_t*)pdt, heap_max, &page);
		unmap((pdt_t*)pdt, heap_max);
		free_page(page);
	}
}

void split_chunk(node_t *chunk, u32 len)
{
	// �з��ڴ��֮ǰ�ñ�֤֮���ʣ���ڴ���������һ���ڴ�����Ĵ�С
	//printf("enter split_chunk,chunk = 0x%08X; len = %d; prev = 0x%08X; next = 0x%08X\n", (u32)chunk, chunk->len, (u32)(chunk->prev), (u32)(chunk->next));
	if (chunk->len - len > sizeof (node_t))
	{
		node_t *newchunk = (node_t *)((u32)chunk + len);
		newchunk->prev = chunk;
		newchunk->next = chunk->next;
		newchunk->allocated = 0;
		newchunk->len = chunk->len - len;

		chunk->next = newchunk;
		chunk->len = len;
	}
}

void merge_block(node_t * block)
{
	// ������ڴ����������ڴ����δ��ʹ����ƴ��
	if (block->next && block->next->allocated == 0)
	{
		block->len = block->len + block->next->len;
		if (block->next->next)
		{
			block->next->next->prev = block;
		}
		block->next = block->next->next;
	}

	// ������ڴ��ǰ�������ڴ����δ��ʹ����ƴ��
	if (block->prev && block->prev->allocated == 0)
	{
		block->prev->len = block->prev->len + block->len;
		block->prev->next = block->next;
		if (block->next)
		{
			block->next->prev = block->prev;
		}
		block = block->prev;
	}

	// ������ڴ����û�������ڴ����ֱ���ͷŵ�
	if (block->next == 0)
	{
		free_block(block);
	}
}

void test_heap()
{
	show_string_color("Test kmalloc() && kfree() now ...\n\n", black, magenta);

	void *addr1 = kmalloc(50);
	printf("kmalloc    50 byte in 0x%08X\n", addr1);
	void *addr2 = kmalloc(500);
	printf("kmalloc   500 byte in 0x%08X\n", addr2);
	void *addr3 = kmalloc(5000);
	printf("kmalloc  5000 byte in 0x%08X\n", addr3);
	void *addr4 = kmalloc(50000);
	printf("kmalloc 50000 byte in 0x%08X\n\n", addr4);

	printf("free mem in 0x%08X\n", addr1);
	kfree(addr1);
	printf("free mem in 0x%08X\n", addr2);
	kfree(addr2);
	printf("free mem in 0x%08X\n", addr3);
	kfree(addr3);
	printf("free mem in 0x%08X\n\n", addr4);
	kfree(addr4);
}

