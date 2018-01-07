/*
filename:	system.h
author:		��ʱ������linux0.11��ʵ��
date:		2018-1
purpose:	�����˽����û�ģʽ�������û�������صĺ꼰����
*/

//// �л����û�ģʽ���С�
// �ú�������iret ָ��ʵ�ִ��ں�ģʽ�л����û�ģʽ����ʼ����0����
#define move_to_user_mode() \
_asm { \
	_asm mov eax,esp				/* �����ջָ��esp ��eax �Ĵ����С�*/\
	_asm push _SELECTOR_KER_DS		/* ���Ƚ���ջ��ѡ���(SS)��ջ��*/\
	_asm push eax					/* Ȼ�󽫱���Ķ�ջָ��ֵ(esp)��ջ��*/\
	_asm pushfd						/* ����־�Ĵ���(eflags)������ջ��*/\
	_asm push _SELECTOR_KER_CS		/* ���ں˴����ѡ���(cs)��ջ��*/\
	_asm push offset l1				/* ��������l1 ��ƫ�Ƶ�ַ(eip)��ջ��*/\
	_asm iretd						/* ִ���жϷ���ָ������ת��������l1 ����*/\
_asm l1: mov eax,_SELECTOR_KER_DS	/* ��ʱ��ʼִ������0��*/\
	_asm mov ds,ax					/* ��ʼ���μĴ���ָ�򱾾ֲ�������ݶΡ�*/\
	_asm mov es,ax \
	_asm mov fs,ax \
	_asm mov gs,ax \
}

#define sti() _asm{ _asm sti }// ���ж�Ƕ����꺯����
//__asm__ ( "sti"::)	
#define cli() _asm{ _asm cli }// ���жϡ�
//__asm__ ( "cli"::)	
#define nop() _asm{ _asm nop }// �ղ�����
//__asm__ ( "nop"::)	
#define iret() _asm{ _asm iretd }// �жϷ��ء�
//__asm__ ( "iret"::)	

//// �������������꺯����
// ������gate_addr -��������ַ��type -��������������ֵ��dpl -��������Ȩ��ֵ��addr -ƫ�Ƶ�ַ��
// %0 - (��dpl,type ��ϳɵ����ͱ�־��)��%1 - (��������4 �ֽڵ�ַ)��
// %2 - (��������4 �ֽڵ�ַ)��%3 - edx(����ƫ�Ƶ�ַaddr)��%4 - eax(�����к��ж�ѡ���)��
void _inline _set_gate(unsigned long *gate_addr, \
					   unsigned short type, \
					   unsigned short dpl, \
					   unsigned long addr) 
{
	// c���ͻ����䶼����ͨ��
	gate_addr[0] = 0x00080000 + (addr & 0xffff);
	gate_addr[1] = 0x8000 + (dpl << 13) + (type << 8) + (addr & 0xffff0000);
}

//// �����ж��ź�����
// ������n - �жϺţ�addr - �жϳ���ƫ�Ƶ�ַ��
// &idt[n]��Ӧ�жϺ����ж����������е�ƫ��ֵ���ж���������������14����Ȩ����0��
#define set_intr_gate(n,addr) \
_set_gate((unsigned long*)(&(idt[n])),14,0,(unsigned long)addr)
//// ���������ź�����
// ������n - �жϺţ�addr - �жϳ���ƫ�Ƶ�ַ��
// &idt[n]��Ӧ�жϺ����ж����������е�ƫ��ֵ���ж���������������15����Ȩ����0��
#define set_trap_gate(n,addr) \
_set_gate((unsigned long*)(&(idt[n])),15,0,(unsigned long)addr)
//// ����ϵͳ�����ź�����
// ������n - �жϺţ�addr - �жϳ���ƫ�Ƶ�ַ��
// &idt[n]��Ӧ�жϺ����ж����������е�ƫ��ֵ���ж���������������15����Ȩ����3��
#define set_system_gate(n,addr) \
_set_gate((unsigned long*)(&(idt[n])),15,3,(unsigned long)addr)
//// ���ö�������������
// ������gate_addr -��������ַ��type -��������������ֵ��dpl -��������Ȩ��ֵ��
// base - �εĻ���ַ��limit - ���޳������μ����������ĸ�ʽ��
#define _set_seg_desc(gate_addr,type,dpl,base,limit) {\
*(gate_addr) = ((base) & 0xff000000) | \
				(((base) & 0x00ff0000) >> 16) | \
				((limit) & 0xf0000) | \
				((dpl) << 13) | \
				(0x00408000) | \
				((type) << 8);/* ��������4 �ֽڡ�*/\
*((gate_addr) + 1) = (((base) & 0x0000ffff) << 16) | \
				((limit) & 0x0ffff);/* ��������4 �ֽڡ�*/ \
}

//// ��ȫ�ֱ�����������״̬��/�ֲ�����������
// ������n - ��ȫ�ֱ�����������n ����Ӧ�ĵ�ַ��addr - ״̬��/�ֲ��������ڴ�Ļ���ַ��
// tp - �������еı�־�����ֽڡ�
// %0 - eax(��ַaddr)��%1 - (��������n �ĵ�ַ)��%2 - (��������n �ĵ�ַƫ��2 ��)��
// %3 - (��������n �ĵ�ַƫ��4 ��)��%4 - (��������n �ĵ�ַƫ��5 ��)��
// %5 - (��������n �ĵ�ַƫ��6 ��)��%6 - (��������n �ĵ�ַƫ��7 ��)��
extern _inline void _set_tssldt_desc(unsigned short *n,unsigned long addr,char tp) 
{ 
	_asm mov ebx,n
	_asm mov ax,104 
	_asm mov word ptr [ebx],ax		// ��TSS ���ȷ���������������(��0-1 �ֽ�)��
	_asm mov eax,addr 
	_asm mov word ptr [ebx+2],ax	// ������ַ�ĵ��ַ�����������2-3 �ֽڡ�
	_asm ror eax,16					// ������ַ��������ax �С�
	_asm mov byte ptr [ebx+4],al	// ������ַ�����е��ֽ�������������4 �ֽڡ�
	_asm mov al,tp
	_asm mov byte ptr [ebx+5],al	// ����־�����ֽ������������ĵ�5 �ֽڡ�
	_asm mov al,0 
	_asm mov byte ptr [ebx+6],al	// �������ĵ�6 �ֽ���0��
	_asm mov byte ptr [ebx+7],ah	// ������ַ�����и��ֽ�������������7 �ֽڡ�
	_asm ror eax,16					// eax ���㡣
}

//// ��ȫ�ֱ�����������״̬����������
// n - �Ǹ���������ָ�룻addr - ���������еĻ���ֵַ������״̬����������������0x89��
#define set_tss_desc(n,addr) \
_set_tssldt_desc((unsigned short*)(n),(unsigned long)(addr), (char)0x89)
//// ��ȫ�ֱ������þֲ�����������
// n - �Ǹ���������ָ�룻addr - ���������еĻ���ֵַ���ֲ�����������������0x82��
#define set_ldt_desc(n,addr) \
_set_tssldt_desc((unsigned short*)(n),(unsigned long)(addr), (char)0x82)

