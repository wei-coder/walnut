/*
filename:	heap.c
author:		wei-coder
date:		2017-12
purpose:	���ڴ�������غ���ʵ��
*/

#include "heap.h"
#include "console.h"

// �����ڴ��
static u32 alloc_chunk(u32 start, u32 len);

// �ͷ��ڴ��
static void free_block(node_t * block);

// �з��ڴ��
static void split_chunk(node_t *chunk, u32 len);

// �ϲ��ڴ��
static void merge_block(node_t * block);

static void init_node(node_t* pNode, u32 len);

/*�ں˶ѵĽ�����ַ*/
static u32 heap_end = HEAP_START;

/*�ں˶ѵ���ʼ��ַ*/
static list_t mem_list = {0};

pdt_t* current_pdt;

void init_heap()
{
	current_pdt = pdt;
	mem_list.head = NULL;
	mem_list.tail = NULL;
}

void init_node(node_t* pNode, u32 len)
{
	if(NULL != pNode)
	{
		pNode->next = NULL;
		pNode->prev = NULL;
		pNode->allocated = 0;
		pNode->len = len - sizeof(node_t);
	}
}

void* kmalloc(u32 len)
{
	node_t* pNode = NULL;
	u32 chunk_start = HEAP_START;

	if(NULL != mem_list.head)
	{
		chunk_start = heap_end;
		pNode = mem_list.head;
		while(pNode)
		{
			if((0 == pNode->allocated) && (pNode->len >= len))
			{
				break;
			}
			pNode = pNode->next;
		}
	}

	if(NULL == pNode)
	{
		int size = alloc_chunk(chunk_start, len);
		pNode = (node_t*)chunk_start;
		init_node(pNode, size);
		if(NULL == mem_list.head)
		{
			mem_list.head = pNode;
			mem_list.tail = pNode;
		}
		else
		{
			mem_list.tail->next = pNode;
			pNode->prev = mem_list.tail;
			mem_list.tail = pNode;
		}
	}

	split_chunk(pNode, len);

	pNode->allocated = 1;
	return (void*)((u32)pNode + sizeof(node_t));
}

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
	//������Ҫ���������������������λ���������룬�����ͷŵĵ�ַҲ���ܱ�Ӧ�á�
	while (start + len > heap_end)
	{
		u32 page = (u32)alloc_page();
		//printf("alloc chunk page = 0x%08X, start= 0x%08X\n", page, start);
		map((pdt_t*)current_pdt, heap_end, page, PAGE_FLAG);
		heap_end += PAGE_SIZE;
		ret += PAGE_SIZE;
	}
	return ret;
}

void free_block(node_t * block)
{
	u32 start = (u32)block;
	u32 end = (u32)block + sizeof(node_t) + block->len;

	/*ҳ�߽����*/
	u32 offset1 = (start - (u32)mem_list.head)%PAGE_SIZE;
	start += offset1;
	u32 offset2 = (end - (u32)mem_list.head)%PAGE_SIZE;
	end -= offset2;

	/*������п�֮�䲻��һҳ�����ͷţ�ֱ�ӷ���*/
	if(end == start)
	{
		return;
	}

	if(0 != offset2)
	{
		node_t* newchunk = (node_t*)end;
		newchunk->allocated = 0;
		newchunk->len = offset2 - sizeof(node_t);
		newchunk->next = block->next;
		if(NULL == newchunk->next)
		{
			mem_list.tail = newchunk;
		}

		if(0 != offset1)
		{
			block->len = offset1 - sizeof(node_t);
			newchunk->prev = block;
			block->next = newchunk;
		}
		else
		{
			if(NULL != block->prev)
			{
				newchunk->prev = block->prev;
				block->prev->next = newchunk;
			}
			else
			{
				mem_list.head = newchunk;
				newchunk->prev = NULL;
			}
		}
	}
	else
	{
		if(0 != offset1)
		{
			block->len = offset1 - sizeof(node_t);
		}
		else
		{
			if(NULL == block->prev)
			{
				mem_list.head = block->next;
			}
		}
		if(NULL == block->next)
		{
			heap_end = start;
		}
		else
		{
			block->next->prev =NULL;
		}
	}
	
	for(int i = 0; i< (end - start)/PAGE_SIZE; i++)
	{
		u32 page;
		get_mapping(current_pdt, (start+i*PAGE_SIZE), &page);
		unmap(current_pdt, (start+i*PAGE_SIZE));
		free_page(page);
	}
}

void split_chunk(node_t *chunk, u32 len)
{
	// �з��ڴ��֮ǰ�ñ�֤֮���ʣ���ڴ���������һ���ڴ�����Ĵ�С
	if ((chunk->len - len) > sizeof (node_t))
	{
		node_t *newchunk = (node_t *)((u32)chunk + len + sizeof(node_t));
		newchunk->prev = chunk;
		newchunk->next = chunk->next;
		if(NULL == newchunk->next)
		{
			mem_list.tail = newchunk;
		}
		newchunk->allocated = 0;
		newchunk->len = chunk->len - len - sizeof(node_t);

		chunk->next = newchunk;
		chunk->len = len;
	}
}

void merge_block(node_t * block)
{
	// ������ڴ����������ڴ����δ��ʹ����ƴ��
	if ((block->next) && (block->next->allocated == 0) )
	{
		block->len = block->len + block->next->len + sizeof(node_t);
		if (block->next->next)
		{
			block->next->next->prev = block;
		}
		block->next = block->next->next;
	}

	// ������ڴ��ǰ�������ڴ����δ��ʹ����ƴ��
	if (block->prev && block->prev->allocated == 0)
	{
		block->prev->len = block->prev->len + block->len + sizeof(node_t);
		block->prev->next = block->next;
		if (block->next)
		{
			block->next->prev = block->prev;
		}
		block = block->prev;
	}

	/*����Ƿ�Ҳ�����Ŀ���ҳ�������ͷ�*/
	free_block(block);
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

void* malloc(u32 len)
{
	return kmalloc(len);
};
void free(void* p)
{
	kfree(p);
	return;
};

