/*
filename: pm.h
author:	wei-coder
date:	2017-12
purpose:	����ģʽ����������ͼ���������
*/

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

/*�����жϴ�����ָ��*/
typedef void (*int_handler_ptr)(int_cont_t *);

/*�����жϺŵĺ�*/
#define		IRQ0		32 	// ����ϵͳ��ʱ��
#define		IRQ1 		33 	// ����
#define		IRQ2 		34 	// �� IRQ9 ��ӣ�MPU-401 MD ʹ��
#define		IRQ3 		35 	// �����豸
#define		IRQ4 		36 	// �����豸
#define		IRQ5 		37 	// ��������ʹ��
#define		IRQ6 		38 	// �����������ʹ��
#define		IRQ7 		39 	// ��ӡ���������ʹ��
#define		IRQ8 		40 	// ��ʱʱ��
#define		IRQ9 		41 	// �� IRQ2 ��ӣ����趨������Ӳ��
#define		IRQ10		42 	// ��������ʹ��
#define		IRQ11		43 	// ���� AGP �Կ�ʹ��
#define		IRQ12		44 	// �� PS/2 ��꣬Ҳ���趨������Ӳ��
#define		IRQ13		45 	// Э������ʹ��
#define		IRQ14		46 	// IDE0 �������ʹ��
#define		IRQ15		47 	// IDE1 �������ʹ��

/*�жϺ�0~19��CPU�쳣�ж�,ISR0~19�Ƕ�Ӧ���жϷ������*/
void isr0(); 			// 0 #DE �� 0 �쳣 
void isr1(); 			// 1 #DB �����쳣 
void isr2(); 			// 2 NMI ���������ж�
void isr3(); 			// 3 BP �ϵ��쳣 
void isr4(); 			// 4 #OF ��� 
void isr5(); 			// 5 #BR ����������ó����߽� 
void isr6(); 			// 6 #UD ��Ч��δ����Ĳ����� 
void isr7(); 			// 7 #NM �豸������(����ѧЭ������) 
void isr8(); 			// 8 #DF ˫�ع���(�д������) 
void isr9(); 			// 9 Э��������β��� 
void isr10(); 		// 10 #TS ��ЧTSS(�д������) 
void isr11(); 		// 11 #NP �β�����(�д������) 
void isr12(); 		// 12 #SS ջ����(�д������) 
void isr13(); 		// 13 #GP ���汣��(�д������) 
void isr14(); 		// 14 #PF ҳ����(�д������) 
void isr15(); 		// 15 CPU ���� 
void isr16(); 		// 16 #MF ���㴦��Ԫ���� 
void isr17(); 		// 17 #AC ������ 
void isr18(); 		// 18 #MC ������� 
void isr19(); 		// 19 #XM SIMD(��ָ�������)�����쳣

/*�жϺ�20~31��intel�����ж�*/
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();

/*�û��Զ����ж�: �жϺ�32~47Ϊ������Ӳ���ж�*/
void irq0();			// ����ϵͳ��ʱ��
void irq1(); 			// ����
void irq2(); 			// �� IRQ9 ��ӣ�MPU-401 MD ʹ��
void irq3(); 			// �����豸
void irq4(); 			// �����豸
void irq5(); 			// ��������ʹ��
void irq6(); 			// �����������ʹ��
void irq7(); 			// ��ӡ���������ʹ��
void irq8(); 			// ��ʱʱ��
void irq9(); 			// �� IRQ2 ��ӣ����趨������Ӳ��
void irq10(); 		// ��������ʹ��
void irq11(); 		// ���� AGP �Կ�ʹ��
void irq12(); 		// �� PS/2 ��꣬Ҳ���趨������Ӳ��
void irq13(); 		// Э������ʹ��
void irq14(); 		// IDE0 �������ʹ��
void irq15(); 		// IDE1 �������ʹ��

/* 48��255 ����ж�*/
void isr255();

/*��ʼ���ж���������*/
void init_idt();

/*��ʼ��ȫ��������*/
void init_gdt();

/*�жϴ�������ע�ắ��*/
void register_int_handler(u8 num, int_handler_ptr func);

void gdt_set_gate(u8 num, u16 flags, u32 base, u32 limit);

void idt_set_gate(u8 num, u32 base, u16 selector, u8 flags);

