#include "types.h"

extern u8 kern_start[];
extern u8 kern_end[];

/*�ں�ջ��С*/
#define STACK_SIZE 8192

/*ҳĿ¼��Ĵ�С*/
#define PDT_LEN 1024

/*ҳ���С*/
#define PTE_LEN	1024

/*����ҳ���С*/
#define PMM_PAGE_SIZE	4096

/*���֧�ֹ���1G�ڴ棬��������ҳ����Ϊ*/
#define PAGE_COUNT	(0x40000000/PMM_PAGE_SIZE)

#define USED		0x01
#define FREE		0x00

/*����ṹ����
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

/*ҳĿ¼�����ṹ*/
typedef struct PDT
{
	union
	{
		u32	directory;
		struct
		{
			u8 present:1;			//�Ƿ��������ڴ���
			u8 rw:1;					//��дȨ��0:ֻ�� 1:��д
			u8 us:1;					//user / supervisor 0:ϵͳ�� 1:�û���
			u8 pwt:1;				//������ԣ�0:write back 1:write through
			u8 pcd:1;				//0:���Ա����壬1:�����Ա�����
			u8 access:1;				//0:δ������ 1:�Ѿ�������
			u8 reserve:1;			//
			u8 page_size:1;			//ҳ��С��0:4K
			u8 global:1;				//ȫ��ҳ
			u8 avail:3;				//����ϵͳ����Աʹ��
			u32 pte_addr:20;			//ҳ�����ַ
		};
	};
}pdt_t;

/*ҳ�����ṹ*/
typedef struct PTE
{
	union
	{
		u32	page;
		struct
		{
			u8 present:1;			//�Ƿ��������ڴ���
			u8 rw:1;					//��дȨ��0:ֻ�� 1:��д
			u8 us:1;					//user / supervisor 0:ϵͳ�� 1:�û���
			u8 pwt:1;				//������ԣ�0:write back 1:write through
			u8 pcd:1;				//0:���Ա����壬1:�����Ա�����
			u8 access:1;				//0:δ������ 1:�Ѿ�������
			u8 dirty:1;				//ҳ�������Ƿ��޸Ĺ���1:�޸Ĺ�
			u8 pat:1;				//page attribute table��Ŀ��p3�Ժ��֧��
			u8 global:1;				//ȫ��ҳ
			u8 avail:3;				//����ϵͳ����Աʹ��
			u32 page_addr:20;		//ҳ����ַ
		};
	};
}pte_t;


void show_mem_map();
void init_pmm();
void init_vmm();

#define PAGE_OFFSET		0xC0000000
#define PAGE_INDEX(x)	(((x) >> 22) & 0x3FF)

