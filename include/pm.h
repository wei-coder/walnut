/*
filename: pm.h
author:	wei-coder
date:	2017-12
purpose:	保护模式相关数据类型及函数声明
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



/*全局描述符表寄存器*/
struct gdtr_t
{
	u16 length;
	u32	base_addr;
}__attribute__((packed));

/*段选择子的结构体定义*/
struct seg_selector
{
	u8	RPL:2;					//段的特权级 分别表示ring0~ring3;
	u8	TI:1;					//TI=0 在GDT查找描述符， TI=1 在LDT查找描述符
	u16	index:13;					//描述符索引，相当于GDT表的下标
}__attribute__((packed));

/*描述符的结构体定义*/
typedef struct desc_struct
{
	union
	{
		struct
		{
			u32	desc_low;						//描述符高32bit
			u32	desc_high;						//描述符低32bit
		};
		struct
		{
			u16	seg_limit_low;					//段界限
			u16	seg_base_low;					//段基址0~15bit
			u8	seg_base_mid;					//段基址16~23
			u8	rw_type:4;						//当b_s=0时表示不同类型的中断门或调用门，=1时表示读写权限
			u8	b_s:1;							//b_s=0 系统段/门描述符；b_s=1 代码段/数据段
			u8	dpl:2;							//段的特权级 分别表示ring0~ring3;
			u8	b_p:1;							//b_p=0 段不在内存；b_p=1 段在内存
			u8	seg_limit_hit:4;					//段界限，与glb_desc.limit_low共同组成20bit的段界限。
			u8	b_avl:1;							//可以被系统软件使用
			u8	b_long:1;						//留给64bit CPU使用
			u8	b_db:1;							//对于代码段，b_db=0 表示使用16bit的偏移地址，即实模式；b_db=1表示使用32bit偏移地址。
												//对于堆栈段，b_db = 0 表示隐式栈操作使用SP, b_db=1表示使用ESP上部边界分别对应0xFFFF,
												//0xFFFFFFFF，
			u8	b_g:1;							//表示段的颗粒度，b_g=0表示1byte，b_g=1表示4K bytes
			u8	seg_base_high;					//段基址24~31
		};
	};
}__attribute__((packed)) desc_t;

/*加载GDTR寄存器*/
extern void gdt_loader();

/*IDTR*/
struct idtr_t
{
	u16 length;
	u32 base;
}__attribute__((packed));

/*门描述符的结构体定义*/
typedef struct gate_desc_struct
{
	union
	{
		struct
		{
			u32	desc_low;					//描述符高32bit
			u32	desc_high;					//描述符低32bit
		};
		struct
		{
			u16	base_low;					//中断处理程序地址的低16bit
			u16	selector;					//段选择子
//			u8	reserved;					//保留位
//			u8	type:4;						//类型
			u8	always0;					//始终为0
//			u8	dpl:2;						//段的特权级 分别表示ring0~ring3;
//			u8	present:1;					//表示有效性，0表示无效。
			u8	flags;
			u16	base_high;					//中断处理程序地址的高16bit
		};
	};
}__attribute__((packed)) gate_desc_t;

/*中断处理上下文*/
typedef struct int_context
{
	u32 ds;				// 用于保存用户的数据段描述符
	u32 edi; 			// 从 edi 到 eax 由 pusha 指令压入
	u32 esi; 
	u32 ebp;
	u32 esp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;
	u32 int_no; 		// 中断号
	u32 err_code;  		// 错误代码(有中断错误代码的中断会由CPU压入)
	u32 eip;			// 以下由处理器自动压入
	u32 cs; 		
	u32 eflags;
	u32 useresp;
	u32 ss;
}int_cont_t;

/*定义中断处理函数指针*/
typedef void (*int_handler_ptr)(int_cont_t *);

/*定义中断号的宏*/
#define		IRQ0		32 	// 电脑系统计时器
#define		IRQ1 		33 	// 键盘
#define		IRQ2 		34 	// 与 IRQ9 相接，MPU-401 MD 使用
#define		IRQ3 		35 	// 串口设备
#define		IRQ4 		36 	// 串口设备
#define		IRQ5 		37 	// 建议声卡使用
#define		IRQ6 		38 	// 软驱传输控制使用
#define		IRQ7 		39 	// 打印机传输控制使用
#define		IRQ8 		40 	// 即时时钟
#define		IRQ9 		41 	// 与 IRQ2 相接，可设定给其他硬件
#define		IRQ10		42 	// 建议网卡使用
#define		IRQ11		43 	// 建议 AGP 显卡使用
#define		IRQ12		44 	// 接 PS/2 鼠标，也可设定给其他硬件
#define		IRQ13		45 	// 协处理器使用
#define		IRQ14		46 	// IDE0 传输控制使用
#define		IRQ15		47 	// IDE1 传输控制使用

/*中断号0~19是CPU异常中断,ISR0~19是对应的中断服务程序*/
void isr0(); 			// 0 #DE 除 0 异常 
void isr1(); 			// 1 #DB 调试异常 
void isr2(); 			// 2 NMI 不可屏蔽中断
void isr3(); 			// 3 BP 断点异常 
void isr4(); 			// 4 #OF 溢出 
void isr5(); 			// 5 #BR 对数组的引用超出边界 
void isr6(); 			// 6 #UD 无效或未定义的操作码 
void isr7(); 			// 7 #NM 设备不可用(无数学协处理器) 
void isr8(); 			// 8 #DF 双重故障(有错误代码) 
void isr9(); 			// 9 协处理器跨段操作 
void isr10(); 		// 10 #TS 无效TSS(有错误代码) 
void isr11(); 		// 11 #NP 段不存在(有错误代码) 
void isr12(); 		// 12 #SS 栈错误(有错误代码) 
void isr13(); 		// 13 #GP 常规保护(有错误代码) 
void isr14(); 		// 14 #PF 页故障(有错误代码) 
void isr15(); 		// 15 CPU 保留 
void isr16(); 		// 16 #MF 浮点处理单元错误 
void isr17(); 		// 17 #AC 对齐检查 
void isr18(); 		// 18 #MC 机器检查 
void isr19(); 		// 19 #XM SIMD(单指令多数据)浮点异常

/*中断号20~31是intel保留中断*/
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

/*用户自定义中断: 中断号32~47为可屏蔽硬件中断*/
void irq0();			// 电脑系统计时器
void irq1(); 			// 键盘
void irq2(); 			// 与 IRQ9 相接，MPU-401 MD 使用
void irq3(); 			// 串口设备
void irq4(); 			// 串口设备
void irq5(); 			// 建议声卡使用
void irq6(); 			// 软驱传输控制使用
void irq7(); 			// 打印机传输控制使用
void irq8(); 			// 即时时钟
void irq9(); 			// 与 IRQ2 相接，可设定给其他硬件
void irq10(); 		// 建议网卡使用
void irq11(); 		// 建议 AGP 显卡使用
void irq12(); 		// 接 PS/2 鼠标，也可设定给其他硬件
void irq13(); 		// 协处理器使用
void irq14(); 		// IDE0 传输控制使用
void irq15(); 		// IDE1 传输控制使用

/* 48～255 软件中断*/
void isr255();

/*初始化中断描述符表*/
void init_idt();

/*初始化全局描述符*/
void init_gdt();

/*中断处理函数的注册函数*/
void register_int_handler(u8 num, int_handler_ptr func);

void gdt_set_gate(u8 num, u16 flags, u32 base, u32 limit);

void idt_set_gate(u8 num, u32 base, u16 selector, u8 flags);

