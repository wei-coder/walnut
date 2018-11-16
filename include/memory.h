
#ifndef __MEMORY_H
#define __MEMORY_H

#include "types.h"
#include "multiboot.h"

extern u8 kern_start[];
extern u8 kern_end[];

/*内核栈大小*/
#define STACK_SIZE 8192

/*页目录表的大小*/
#define PDT_LEN 1024

/*页表大小*/
#define PTE_LEN	1024

/*物理页框大小*/
#define PAGE_SIZE	4096

/*最多支持管理1G内存，所以物理页数量为*/
#define PAGE_COUNT	(0x40000000/PAGE_SIZE)

#define USED		0x80
#define FREE		0x00

#define 		PDT_FLAG	0x07
#define		PAGE_FLAG	0x07

#define		PROC_PAGE_FLAG	0x07

#define	pdt_t u32
#define pte_t u32

/*高端内存起始地址为0x38000000，低于此地址的内存为直接映射*/
#define HIGH_MEM_ADDR	0x38000000

/*定义线性映射区的PTE个数*/
#define PTE_COUNT	224

/*定义内核空间的起始页目录索引*/
#define KERN_PDT_INDEX	768

/*管理的物理内存起始地址为0x100000*/
#define NOMAL_MEM_ADDR	0x100000


/*页目录表和页表的定义*/
extern pdt_t* pdt;

extern multiboot_t *glb_mboot_ptr;

typedef struct PHY_PAGE
{
	union
	{
		u32	page;
		struct
		{
			u8 used;
			u8 refer;
			u8 reserved1;
			u8 reserved2;
		};
	};
}phy_page;

struct vm_operations_struct
{
	
};

struct vm_area_struct
{
	unsigned long vm_start;		
	unsigned long vm_end;			
	struct vm_area_struct *vm_next;
	unsigned long vm_flags;		
	struct vm_area_struct *vm_next_share;
	struct vm_area_struct **vm_pprev_share;
	struct vm_operations_struct * vm_ops;
	unsigned long vm_pgoff;		
	struct file * vm_file; 		
	unsigned long vm_raend;		
	void * vm_private_data;	  
};



/*页目录表表项结构
typedef struct PDT
{
	union
	{
		u32	directory;
		struct
		{
			u8 present:1;			//是否在物理内存中
			u8 rw:1;				//读写权限0:只读 1:读写
			u8 us:1;				//user / supervisor 0:系统级 1:用户级
			u8 pwt:1;				//缓冲策略，0:write back 1:write through
			u8 pcd:1;				//0:可以被缓冲，1:不可以被缓冲
			u8 access:1;			//0:未被访问 1:已经被访问
			u8 reserve:1;			//
			u8 page_size:1;			//页大小，0:4K
			u8 global:1;			//全局页
			u8 avail:3;				//允许系统程序员使用
			u32 pte_addr:20;		//页表基地址
		};
	};
}pdt_t;
*/

/*页表表项结构
typedef struct PTE
{
	union
	{
		u32	page;
		struct
		{
			u8 present:1;				//是否在物理内存中
			u8 rw:1;					//读写权限0:只读 1:读写
			u8 us:1;					//user / supervisor 0:系统级 1:用户级
			u8 pwt:1;					//缓冲策略，0:write back 1:write through
			u8 pcd:1;					//0:可以被缓冲，1:不可以被缓冲
			u8 access:1;				//0:未被访问 1:已经被访问
			u8 dirty:1;				//页的内容是否修改过，1:修改过
			u8 pat:1;					//page attribute table条目，p3以后才支持
			u8 global:1;				//全局页
			u8 avail:3;				//允许系统程序员使用
			u32 page_addr:20;			//页基地址
		};
	};
}pte_t;
*/

void show_mem_map();
void init_pmm();
void init_vmm();
void* alloc_page();
void map(pdt_t *pgd_now, u32 va, u32 pa, u32 flags);
void unmap(pdt_t *pgd_now, u32 va);
u32 get_mapping(pdt_t *pgd_now, u32 va, u32 *pa);
void free_page(u32 addr );
void free_virt_page(u32 p);
int copy_page_tables (u32* from, u32* to);
int free_page_tables(u32 pdt_addr);
void* get_virt_page();

#define PAGE_OFFSET		0xC0000000
#define PDT_INDEX(x) 	(((x) >> 22) & 0x3FF)
#define PTE_INDEX(x)		(((x) >> 12) & 0x3FF)
#define PAGE_MASK		0xFFFFF000
#define FLAG_MASK		0x00000FFF
#define MAP_INDEX(X)	((X-NOMAL_MEM_ADDR)>>12)

#endif
