/*
filename:	heap.c
author:		wei-coder
date:		2017-12
purpose:	堆内存管理的相关函数实现
*/

#include "heap.h"
#include "console.h"
#include "memory.h"

// 申请内存块
static u32 alloc_chunk(u32 start, u32 len);

// 释放内存块
static void free_block(node_t * block);

// 切分内存块
static void split_chunk(node_t *chunk, u32 len);

// 合并内存块
static void merge_block(node_t * block);

static void init_node(node_t* pNode, u32 len);

static u32 heap_max = HEAP_START;

// 内存块管理头指针
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
			// 按照当前长度切割内存
			split_chunk(pNode, len);
			pNode->allocated = 1;
			// 返回的时候必须将指针挪到管理结构之后
			return (void *)((u32)pNode + sizeof(node_t));
		}
		// 逐次推移指针
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
	// 所有申请的内存长度加上管理头的长度
	// 因为在内存申请和释放的时候要通过该结构去管理
	len += sizeof(node_t);

	node_t * p_node = phead;
	node_t * prev_node = NULL;

	while (p_node)
	{
		// 如果当前内存块没有被申请过而且长度大于待申请的块
		if (p_node->allocated == 0 && p_node->len >= len)
		{
			// 按照当前长度切割内存
			split_chunk(p_node, len);
			p_node->allocated = 1;
			// 返回的时候必须将指针挪到管理结构之后
			return (void *)((u32)p_node + sizeof(node_t));
		}
		// 逐次推移指针
		prev_node = p_node;
		p_node = p_node->next;
		printf("prev_node addr = 0x%08X; p_node addr = 0x%08X \n", prev_node, p_node);
	}

	u32 chunk_start;

	// 第一次执行该函数则初始化内存块起始位置
	// 之后根据当前指针加上申请的长度即可
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
	
	// 检查是否需要申请内存页
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
	// 指针回退到管理结构，并将已使用标记置 0
	node_t *header = (node_t*)((u32)p - sizeof(node_t));
	header->allocated = 0;

	// 合并内存块
	merge_block(header);
}

u32 alloc_chunk(u32 start, u32 len)
{
	u32 ret = 0;
	// 如果当前堆的位置已经到达界限则申请内存页
	// 必须循环申请内存页直到有到足够的可用内存
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

	// 空闲的内存超过 1 页的话就释放掉
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
	// 切分内存块之前得保证之后的剩余内存至少容纳一个内存管理块的大小
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
	// 如果该内存块后面有链内存块且未被使用则拼合
	if (block->next && block->next->allocated == 0)
	{
		block->len = block->len + block->next->len;
		if (block->next->next)
		{
			block->next->next->prev = block;
		}
		block->next = block->next->next;
	}

	// 如果该内存块前面有链内存块且未被使用则拼合
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

	// 假如该内存后面没有链表内存块了直接释放掉
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

