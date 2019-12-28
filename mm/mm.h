
#ifndef __MEMORY_H
#define __MEMORY_H

#include "types.h"
#include "multiboot.h"

extern u8 kern_start[];
extern u8 kern_end[];

/*�ں�ջ��С*/
#define STACK_SIZE 8192

/*ҳĿ¼��Ĵ�С*/
#define PDT_LEN 1024

/*ҳ���С*/
#define PTE_LEN	1024

/*����ҳ���С*/
#define PAGE_SIZE	4096

/*���֧�ֹ���1G�ڴ棬��������ҳ����Ϊ*/
#define PAGE_COUNT	(0x40000000/PAGE_SIZE)

#define USED		0x80
#define FREE		0x00

#define 		PDT_FLAG	0x07
#define		PAGE_FLAG	0x07

#define		PROC_PAGE_FLAG	0x07

#define	pdt_t u32
#define pte_t u32

/*�߶��ڴ���ʼ��ַΪ0x38000000�����ڴ˵�ַ���ڴ�Ϊֱ��ӳ��*/
#define HIGH_MEM_ADDR	0x38000000

/*��������ӳ������PTE����*/
#define PTE_COUNT	224

/*�����ں˿ռ����ʼҳĿ¼����*/
#define KERN_PDT_INDEX	768

/*����������ڴ���ʼ��ַΪ0x100000*/
#define NOMAL_MEM_ADDR	0x100000


/*ҳĿ¼���ҳ��Ķ���*/
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



/*ҳĿ¼�����ṹ
typedef struct PDT
{
	union
	{
		u32	directory;
		struct
		{
			u8 present:1;			//�Ƿ��������ڴ���
			u8 rw:1;				//��дȨ��0:ֻ�� 1:��д
			u8 us:1;				//user / supervisor 0:ϵͳ�� 1:�û���
			u8 pwt:1;				//������ԣ�0:write back 1:write through
			u8 pcd:1;				//0:���Ա����壬1:�����Ա�����
			u8 access:1;			//0:δ������ 1:�Ѿ�������
			u8 reserve:1;			//
			u8 page_size:1;			//ҳ��С��0:4K
			u8 global:1;			//ȫ��ҳ
			u8 avail:3;				//����ϵͳ����Աʹ��
			u32 pte_addr:20;		//ҳ�����ַ
		};
	};
}pdt_t;
*/

/*ҳ�����ṹ
typedef struct PTE
{
	union
	{
		u32	page;
		struct
		{
			u8 present:1;				//�Ƿ��������ڴ���
			u8 rw:1;					//��дȨ��0:ֻ�� 1:��д
			u8 us:1;					//user / supervisor 0:ϵͳ�� 1:�û���
			u8 pwt:1;					//������ԣ�0:write back 1:write through
			u8 pcd:1;					//0:���Ա����壬1:�����Ա�����
			u8 access:1;				//0:δ������ 1:�Ѿ�������
			u8 dirty:1;				//ҳ�������Ƿ��޸Ĺ���1:�޸Ĺ�
			u8 pat:1;					//page attribute table��Ŀ��p3�Ժ��֧��
			u8 global:1;				//ȫ��ҳ
			u8 avail:3;				//����ϵͳ����Աʹ��
			u32 page_addr:20;			//ҳ����ַ
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
