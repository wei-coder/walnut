<<<<<<< HEAD
;filename:page.s
;author:  wei-coder

[GLOBAL page_fault]
[EXTERN do_no_page]
[EXTERN do_wp_page]
page_fault:
	xchg [esp],eax
	push ecx
	push edx
	push ds
	push es
	push fs
	mov edx, 0x68
	mov ds,dx
	mov es,dx
	mov fs,dx
	mov edx,cr2
	push edx
	push eax
	test eax,1
	jne .1
	call do_no_page
	jmp .2
.1:	call do_wp_page
.2:	add esp,8
	pop fs
	pop es
	pop ds
	pop edx
	pop ecx
	pop eax
	iret

=======
;filename: page.s
;author:   wei-coder
;date:     2018-2

_SELECTOR_KER_DS	equ	0x68


[GLOBAL page_fault]
[EXTERN do_no_page]
[EXTERN do_wp_page]
page_fault:
	xchg [esp],eax       ;# ȡ�����뵽eax��
	push ecx
	push edx
	push ds
	push es
	push fs
	mov edx,_SELECTOR_KER_DS         ;# ���ں����ݶ�ѡ�����
	mov ds,dx
	mov es,dx
	mov fs,dx
	mov edx,cr2          ;# ȡ����ҳ���쳣�����Ե�ַ
	push edx              ;# �������Ե�ַ�ͳ�����ѹ���ջ����Ϊ���ú����Ĳ�����
	push eax
	test eax,1           ;# ���Ա�־P���������ȱҳ������쳣����ת��
	jne .1
	call do_no_page        ;# ����ȱҳ������
	jmp .2
.1:	call do_wp_page        ;# ����д����������
.2:	add esp,8            ;# ����ѹ��ջ������������
	pop fs
	pop es
	pop ds
	pop edx
	pop ecx
	pop eax
	iret

>>>>>>> 2747b789ab05c6901e2e08b9db7fbb72fd8f6f24
