/*
filename: pm.h
author:	wei-coder
date:	2017-12
purpose:	����ģʽ����������ͼ���������
*/

#ifndef __pm_h__
#define __pm_h__

#include "types.h"

#define GDT_ENTRY_LEN   32
#define IDT_ENTRY_LEN   256

#define	RING0	0x00
#define 	RING1	0x01
#define	RING2	0x02
#define	RING3	0x03

#define	GDT_INDEX_NULL			0
#define	GDT_INDEX_KERNEL_CS		12
#define 	GDT_INDEX_KERNEL_DS		13
#define	GDT_INDEX_USER_CS			14
#define	GDT_INDEX_USER_DS		15
#define 	GDT_INDEX_TSS				16
#define	GDT_INDEX_LDT				17
#define 	GDT_INDEX_DFAULT_TSS		31

#define KERNEL_CS_FLAG			0xCF9A
#define KERNEL_DS_FLAG			0xCF92
#define USER_CS_FLAG			0xCFFA
#define USER_DS_FLAG			0xCFF2
#define TSS_FLAG				0xCF89
#define LDT_FLAG				0xCF82
#define KERNEL_CS_BASE			0
#define KERNEL_DS_BASE			0
#define USER_CS_BASE			0
#define USER_DS_BASE			0
#define TSS_BASE				0
#define LDT_BASE				0
#define KERNEL_CS_LIMIT			0xFFFFFFFF
#define KERNEL_DS_LIMIT			0xFFFFFFFF
#define USER_CS_LIMIT			0xFFFFFFFF
#define USER_DS_LIMIT			0xFFFFFFFF
#define TSS_LIMIT				0xFFFFFFFF
#define LDT_LIMIT				0xFFFFFFFF

#define _SELECTOR_KER_CS		(GDT_INDEX_KERNEL_CS*8)
#define _SELECTOR_KER_DS		(GDT_INDEX_KERNEL_DS*8)
#define _SELECTOR_USER_CS		(GDT_INDEX_USER_CS*8 + 3)
#define _SELECTOR_USER_DS		(GDT_INDEX_USER_DS*8 + 3)
#define	_SELECTOR_TSS			(GDT_INDEX_TSS*8)
#define	_SELECTOR_LDT			(GDT_INDEX_LDT*8 + 7)




#define	ISR_DIV_ZERO		0	// 0 #DE �� 0 �쳣 
#define	ISR_DEBUG_ERR		1	// 1 #DB �����쳣 
#define	ISR_NMI				2	// 2 NMI 
#define	ISR_BP				3	// 3 BP �ϵ��쳣 
#define	ISR_OF				4	// 4 #OF ��� 
#define	ISR_ARR_OVER		5	// 5 #BR ����������ó����߽� 
#define	ISR_ERR_CODE		6	// 6 #UD ��Ч��δ����Ĳ����� 
#define	ISR_NM_INVALID		7	// 7 #NM �豸������(����ѧЭ������) 
#define	ISR_DOUB_FAULT	8	// 8 #DF ˫�ع���(�д������) 
#define	ISR_COPROCESS_ERR  9	// 9 Э��������β��� 
#define 	ISR_TSS_INVALID	10	// 10 #TS ��ЧTSS(�д������) 
#define 	ISR_NP_NOT_EXSIT	11	// 11 #NP �β�����(�д������) 
#define 	ISR_STACK_ERR		12	// 12 #SS ջ����(�д������) 
#define 	ISR_GEN_PROTECT	13	// 13 #GP ���汣��(�д������) 
#define 	ISR_PAGE_FAULT	14	// 14 #PF ҳ����(�д������) 
#define 	ISR_RESERVE		15	// 15 CPU ���� 
#define	ISR_MF				16	// 16 #MF ���㴦��Ԫ���� 
#define	ISR_AC				17	// 17 #AC ������ 
#define	ISR_MC 				18	// 18 #MC ������� 
#define	ISR_SIMD			19	// 19 #XM SIMD(��ָ�������)�����쳣

#define  IRQ_TIMER			32 	// ����ϵͳ��ʱ��
#define  IRQ_KEYBOARD		33 	// ����
#define  IRQ2					34 	// �� IRQ9 ��ӣ�MPU-401 MD ʹ��
#define  IRQ_SERIAL1			35 	// �����豸
#define  IRQ_SERIAL2			36 	// �����豸
#define  IRQ_SOUND			37 	// ��������ʹ��
#define  IRQ_FLOPPY			38 	// �����������ʹ��
#define  IRQ_PRINTER			39 	// ��ӡ���������ʹ��
#define  IRQ_REALTIME		40 	// ��ʱʱ��
#define  IRQ9					41 	// �� IRQ2 ��ӣ����趨������Ӳ��
#define  IRQ_NETCARD		42 	// ��������ʹ��
#define  IRQ_AGP_VIDEO		43 	// ���� AGP �Կ�ʹ��
#define  IRQ_MOUSE			44 	// �� PS/2 ��꣬Ҳ���趨������Ӳ��
#define  IRQ_COPROCESS		45 	// Э������ʹ��
#define  IRQ_IDE0_HD		46 	// IDE0 �������ʹ��
#define  IRQ_IDE1_HD		47 	// IDE1 �������ʹ��


/*ȫ����������Ĵ���*/
struct gdtr_t
{
	u16 length;
	u32	base_addr;
}__attribute__((packed));

/*��ѡ���ӵĽṹ�嶨��*/
struct seg_selector
{
	u8	RPL:2;					//�ε���Ȩ�� �ֱ��ʾring0~ring3;
	u8	TI:1;					//TI=0 ��GDT������������ TI=1 ��LDT����������
	u16	index:13;					//�������������൱��GDT����±�
}__attribute__((packed));

/*�������Ľṹ�嶨��*/
typedef struct desc_struct
{
	union
	{
		struct
		{
			u32	desc_low;						//��������32bit
			u32	desc_high;						//��������32bit
		};
		struct
		{
			u16	seg_limit_low;					//�ν���
			u16	seg_base_low;					//�λ�ַ0~15bit
			u8	seg_base_mid;					//�λ�ַ16~23
			u8	rw_type:4;						//��b_s=0ʱ��ʾ��ͬ���͵��ж��Ż�����ţ�=1ʱ��ʾ��дȨ��
			u8	b_s:1;							//b_s=0 ϵͳ��/����������b_s=1 �����/���ݶ�
			u8	dpl:2;							//�ε���Ȩ�� �ֱ��ʾring0~ring3;
			u8	b_p:1;							//b_p=0 �β����ڴ棻b_p=1 �����ڴ�
			u8	seg_limit_hit:4;					//�ν��ޣ���glb_desc.limit_low��ͬ���20bit�Ķν��ޡ�
			u8	b_avl:1;							//���Ա�ϵͳ���ʹ��
			u8	b_long:1;						//����64bit CPUʹ��
			u8	b_db:1;							//���ڴ���Σ�b_db=0 ��ʾʹ��16bit��ƫ�Ƶ�ַ����ʵģʽ��b_db=1��ʾʹ��32bitƫ�Ƶ�ַ��
												//���ڶ�ջ�Σ�b_db = 0 ��ʾ��ʽջ����ʹ��SP, b_db=1��ʾʹ��ESP�ϲ��߽�ֱ��Ӧ0xFFFF,
												//0xFFFFFFFF��
			u8	b_g:1;							//��ʾ�εĿ����ȣ�b_g=0��ʾ1byte��b_g=1��ʾ4K bytes
			u8	seg_base_high;					//�λ�ַ24~31
		};
	};
}__attribute__((packed)) desc_t;

/*����GDTR�Ĵ���*/
extern void gdt_loader();

/*IDTR*/
struct idtr_t
{
	u16 length;
	u32 base;
}__attribute__((packed));

/*���������Ľṹ�嶨��*/
typedef struct gate_desc_struct
{
	union
	{
		struct
		{
			u32	desc_low;					//��������32bit
			u32	desc_high;					//��������32bit
		};
		struct
		{
			u16	base_low;					//�жϴ�������ַ�ĵ�16bit
			u16	selector;					//��ѡ����
//			u8	reserved;					//����λ
//			u8	type:4;						//����
			u8	always0;					//ʼ��Ϊ0
//			u8	dpl:2;						//�ε���Ȩ�� �ֱ��ʾring0~ring3;
//			u8	present:1;					//��ʾ��Ч�ԣ�0��ʾ��Ч��
			u8	flags;
			u16	base_high;					//�жϴ�������ַ�ĸ�16bit
		};
	};
}__attribute__((packed)) gate_desc_t;

/*�жϴ���������*/
typedef struct int_context
{
	u32 ds;				// ���ڱ����û������ݶ�������
	u32 edi; 			// �� edi �� eax �� pusha ָ��ѹ��
	u32 esi; 
	u32 ebp;
	u32 esp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;
	u32 int_no; 		// �жϺ�
	u32 err_code;  		// �������(���жϴ��������жϻ���CPUѹ��)
	u32 eip;			// �����ɴ������Զ�ѹ��
	u32 cs; 		
	u32 eflags;
	u32 useresp;
	u32 ss;
}int_cont_t;

/*�ж���������Ķ��弰��ʼ��*/
extern gate_desc_t idt_entry[IDT_ENTRY_LEN];

/*��ʼ���ж���������*/
void init_idt();

/*��ʼ��ȫ��������*/
void init_gdt();

void gdt_set_gate(u8 num, u16 flags, u32 base, u32 limit);

void idt_set_gate(u8 num, u32 base, u16 selector, u8 flags);

#endif
