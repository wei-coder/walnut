;filename: pm.s
;author:   wei-coder
;date:	   2017-12
;purpose:  ���뱣��ģʽ


[GLOBAL gdt_loader]

gdt_loader:
	mov eax, [esp+4]  ; �������� eax �Ĵ���
	lgdt [eax]        ; ���ص� GDTR [�޸�ԭ��GRUB����]

	mov ax, 0x10      ; �������ݶ�������
	mov ds, ax        ; �������п��Ը��µĶμĴ���
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.flush   ; Զ��ת��0x08�Ǵ����������
			  ; Զ��Ŀ���������ˮ�߲����л�������
.flush:
	ret
