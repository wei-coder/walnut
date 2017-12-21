#include "types.h"
#include "multiboot.h"

extern u8 kern_start[];
extern u8 kern_end[];

/*页目录表的大小*/
#define PDT_LEN 1024

/*页表大小*/
#define PTE_LEN	1024

/*物理页框大小*/
#define PMM_PAGE_SIZE	4*1024

/*最多支持管理1G内存，所以物理页数量为*/
#define PAGE_COUNT	0x40000000/PMM_PAGE_SIZE


typedef struct NODE
{
	u32 base;
	u32	len;
	struct NODE* next;
}node_t;

typedef struct LIST
{
	node_t * head;
	node_t * tail;
}list_t;

void show_mem_map();

