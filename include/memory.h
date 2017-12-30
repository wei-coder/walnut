#include "types.h"

extern u8 kern_start[];
extern u8 kern_end[];

/*内核栈大小*/
#define STACK_SIZE 8192

/*页目录表的大小*/
#define PDT_LEN 1024

/*页表大小*/
#define PTE_LEN	1024

/*物理页框大小*/
#define PMM_PAGE_SIZE	4096

/*最多支持管理1G内存，所以物理页数量为*/
#define PAGE_COUNT	(0x40000000/PMM_PAGE_SIZE)

#define USED		0x01
#define FREE		0x00

/*链表结构定义
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
*/

#define 	PDT_FLAG	0x03
#define	PAGE_FLAG	0x03

/*页目录表表项结构*/
typedef struct PDT
{
	union
	{
		u32	directory;
		struct
		{
			u8 present:1;			//是否在物理内存中
			u8 rw:1;					//读写权限0:只读 1:读写
			u8 us:1;					//user / supervisor 0:系统级 1:用户级
			u8 pwt:1;				//缓冲策略，0:write back 1:write through
			u8 pcd:1;				//0:可以被缓冲，1:不可以被缓冲
			u8 access:1;				//0:未被访问 1:已经被访问
			u8 reserve:1;			//
			u8 page_size:1;			//页大小，0:4K
			u8 global:1;				//全局页
			u8 avail:3;				//允许系统程序员使用
			u32 pte_addr:20;			//页表基地址
		};
	};
}pdt_t;

/*页表表项结构*/
typedef struct PTE
{
	union
	{
		u32	page;
		struct
		{
			u8 present:1;			//是否在物理内存中
			u8 rw:1;					//读写权限0:只读 1:读写
			u8 us:1;					//user / supervisor 0:系统级 1:用户级
			u8 pwt:1;				//缓冲策略，0:write back 1:write through
			u8 pcd:1;				//0:可以被缓冲，1:不可以被缓冲
			u8 access:1;				//0:未被访问 1:已经被访问
			u8 dirty:1;				//页的内容是否修改过，1:修改过
			u8 pat:1;				//page attribute table条目，p3以后才支持
			u8 global:1;				//全局页
			u8 avail:3;				//允许系统程序员使用
			u32 page_addr:20;		//页基地址
		};
	};
}pte_t;


void show_mem_map();
void init_pmm();
void init_vmm();

#define PAGE_OFFSET		0xC0000000
#define PAGE_INDEX(x)	(((x) >> 22) & 0x3FF)

