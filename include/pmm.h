#include "types.h"
#include "multiboot.h"

extern u8 kern_start[];
extern u8 kern_end[];

/*ҳĿ¼��Ĵ�С*/
#define PDT_LEN 1024

/*ҳ���С*/
#define PTE_LEN	1024

/*����ҳ���С*/
#define PMM_PAGE_SIZE	4*1024

/*���֧�ֹ���1G�ڴ棬��������ҳ����Ϊ*/
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

