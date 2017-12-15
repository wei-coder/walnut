; ----------------------------------------------------------------
;
; 	boot.s -- �ں˴����￪ʼ
;
;                 ���ﻹ�и��� GRUB Multiboot �淶��һЩ����
;
; ----------------------------------------------------------------

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

section .init.text 	; ��ʱ����δ����￪ʼ

; �ڴ���ε���ʼλ�����÷��� Multiboot �淶�ı��

dd MBOOT_HEADER_MAGIC 	; GRUB ��ͨ�����ħ���жϸ�ӳ���Ƿ�֧��
dd MBOOT_HEADER_FLAGS   ; GRUB ��һЩ����ʱѡ�����ϸע���ڶ��崦
dd MBOOT_CHECKSUM       ; �����ֵ���京���ڶ��崦

[GLOBAL start] 		; �ں˴�����ڣ��˴��ṩ�������� ld ������
[GLOBAL mboot_ptr_tmp] 	; ȫ�ֵ� struct multiboot * ����
[EXTERN kern_entry] 	; �����ں� C �������ں���

start:
	cli  				; ��ʱ��û�����úñ���ģʽ���жϴ������Ա���ر��ж�
	mov [mboot_ptr_tmp], ebx	; �� ebx �д洢��ָ����� glb_mboot_ptr ����
	mov esp, STACK_TOP  		; �����ں�ջ��ַ������ multiboot �淶������Ҫʹ�ö�ջʱ��OS ӳ������Լ�����һ��
	and esp, 0FFFFFFF0H		; ջ��ַ���� 16 �ֽڶ���
	mov ebp, 0 			; ָ֡���޸�Ϊ 0
    
	call kern_entry	; �����ں���ں���

;-----------------------------------------------------------------------------

section .init.data		; ������ҳǰ��ʱ�����ݶ�
stack:    times 1024 db 0  	; ������Ϊ��ʱ�ں�ջ
STACK_TOP equ $-stack-1 	; �ں�ջ����$ ��ָ���ǵ�ǰ��ַ

mboot_ptr_tmp: dd 0		; ȫ�ֵ� multiboot �ṹ��ָ��

;-----------------------------------------------------------------------------
