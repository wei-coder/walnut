;filename: boot.s
;author:    wei-coder
;date:	  2017-12
;purpose:  �˴���OS�����Ĵ�����ڣ�����ͨ������multiboot�ı�׼�ӿڣ���GRUBʶ����á�

MBOOT_HEADER_MAGIC 	equ 	0x1BADB002 	; Multiboot ħ�����ɹ淶������

MBOOT_PAGE_ALIGN 	equ 	1 << 0    	; 0 ��λ��ʾ���е�����ģ�齫��ҳ(4KB)�߽����
MBOOT_MEM_INFO 		equ 	1 << 1    	; 1 ��λͨ�� Multiboot ��Ϣ�ṹ�� mem_* ����������ڴ����Ϣ
						; (����GRUB���ڴ�ռ����Ϣ������Multiboot��Ϣ�ṹ��)

; ��������ʹ�õ� Multiboot �ı��
MBOOT_HEADER_FLAGS 	equ 	MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO

; ��checksum��һ��32λ���޷���ֵ������������magic��(Ҳ����magic��flags)���ʱ��
; Ҫ������������32λ���޷���ֵ 0 (��magic + flags + checksum = 0)
MBOOT_CHECKSUM 		equ 	- (MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

; ����Multiboot�淶�� OS ӳ����Ҫ����һ�� magic Multiboot ͷ

; Multiboot ͷ�ķֲ��������±���ʾ��
; ----------------------------------------------------------
; ƫ����  ����  ����        ��ע
;
;   0     u32   magic       ����
;   4     u32   flags       ���� 
;   8     u32   checksum    ���� 
;
; ����ֻʹ�õ���Щ�͹��ˣ��������ϸ˵������� GNU ����ĵ�
;-----------------------------------------------------------

;-----------------------------------------------------------------------------

[BITS 32]  	; ���д����� 32-bit �ķ�ʽ����

section .text 	; ����δ����￪ʼ

; �ڴ���ε���ʼλ�����÷��� Multiboot �淶�ı��

dd MBOOT_HEADER_MAGIC 	; GRUB ��ͨ�����ħ���жϸ�ӳ���Ƿ�֧��
dd MBOOT_HEADER_FLAGS   ; GRUB ��һЩ����ʱѡ�����ϸע���ڶ��崦
dd MBOOT_CHECKSUM       ; �����ֵ���京���ڶ��崦

[GLOBAL start] 		; �ں˴�����ڣ��˴��ṩ�������� ld ������
[GLOBAL glb_mboot_ptr] 	; ȫ�ֵ� struct multiboot * ����
[EXTERN main] 	; �����ں� C �������ں���

start:
	cli  			 ; ��ʱ��û�����úñ���ģʽ���жϴ���Ҫ�ر��ж�
				 ; ���Ա���ر��ж�
	mov esp, STACK_TOP  	 ; �����ں�ջ��ַ
	mov ebp, 0 		 ; ָ֡���޸�Ϊ 0
	and esp, 0FFFFFFF0H	 ; ջ��ַ����16�ֽڶ���
	mov [glb_mboot_ptr], ebx ; �� ebx �д洢��ָ�����ȫ�ֱ���
	call main		 ; �����ں���ں���
stop:
	hlt 			 ; ͣ��ָ�ʲôҲ���������Խ��� CPU ����
	jmp stop 		 ; ������������ػ�ʲô�ĺ�����˵

;-----------------------------------------------------------------------------

section .bss 			 ; δ��ʼ�������ݶδ����￪ʼ
stack:
	resb 32768 	 	 ; ������Ϊ�ں�ջ
glb_mboot_ptr: 			 ; ȫ�ֵ� multiboot �ṹ��ָ��
	resb 4

STACK_TOP equ $-stack-1 	 ; �ں�ջ����$ ��ָ���ǵ�ǰ��ַ

;-----------------------------------------------------------------------------


