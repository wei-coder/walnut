;filename: pm.s
;author:   wei-coder
;date:	   2017-12
;purpose:  ���뱣��ģʽ

_SELECTOR_KER_DS	equ 	0x68
_SELECTOR_KER_CS	equ	0x60

[GLOBAL gdt_loader]

gdt_loader:
	mov eax, [esp+4]  		; �������� eax �Ĵ���
	lgdt [eax]        		; ���ص� GDTR [�޸�ԭ��GRUB����]

	mov ax, _SELECTOR_KER_DS	; �������ݶ�������
	mov ds, ax       		; �������п��Ը��µĶμĴ���
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp _SELECTOR_KER_CS:.flush   	; Զ��ת��0x60�Ǵ����������
			  		; Զ��Ŀ���������ˮ�߲����л�������
.flush:
	ret
