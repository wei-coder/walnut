;filename:	traps.s
;author:	wei-coder
;date:		2017-12
;purpose:	�жϼ����ֵ����š������ŵ���ں�����Ŀǰ��ʵ�ֽ�����hurlex��idt.s��ʵ�֣�Ŀ���ǿ��ٴ�һ�����ӡ�

; ����û�д��������ж�
%macro ISR_NOERRCODE 1
[GLOBAL isr%1]
isr%1:
	cli                         ; ���ȹر��ж�
	push 0                      ; push ��Ч���жϴ������(��ռλ���ã���������isr����ͳһ��ջ)
	push %1                     ; push �жϺ�
	jmp isr_common_stub
%endmacro

; �����д��������ж�
%macro ISR_ERRCODE 1
[GLOBAL isr%1]
isr%1:
	cli                         ; �ر��ж�
	push %1                     ; push �жϺ�
	jmp isr_common_stub
%endmacro

; �����жϴ�����
ISR_NOERRCODE  0 	; 0 #DE �� 0 �쳣
ISR_NOERRCODE  1 	; 1 #DB �����쳣
ISR_NOERRCODE  2 	; 2 NMI
ISR_NOERRCODE  3 	; 3 BP �ϵ��쳣 
ISR_NOERRCODE  4 	; 4 #OF ��� 
ISR_NOERRCODE  5 	; 5 #BR ����������ó����߽� 
ISR_NOERRCODE  6 	; 6 #UD ��Ч��δ����Ĳ����� 
ISR_NOERRCODE  7 	; 7 #NM �豸������(����ѧЭ������) 
ISR_ERRCODE    8 	; 8 #DF ˫�ع���(�д������) 
ISR_NOERRCODE  9 	; 9 Э��������β���
ISR_ERRCODE   10 	; 10 #TS ��ЧTSS(�д������) 
ISR_ERRCODE   11 	; 11 #NP �β�����(�д������) 
ISR_ERRCODE   12 	; 12 #SS ջ����(�д������) 
ISR_ERRCODE   13 	; 13 #GP ���汣��(�д������) 
ISR_ERRCODE   14 	; 14 #PF ҳ����(�д������) 
ISR_NOERRCODE 15 	; 15 CPU ���� 
ISR_NOERRCODE 16 	; 16 #MF ���㴦��Ԫ���� 
ISR_ERRCODE   17 	; 17 #AC ������ 
ISR_NOERRCODE 18 	; 18 #MC ������� 
ISR_NOERRCODE 19 	; 19 #XM SIMD(��ָ�������)�����쳣

; 20~31 Intel ����
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
; 32��255 �û��Զ���
ISR_NOERRCODE 255

[GLOBAL isr_common_stub]
[EXTERN isr_handler]
; �жϷ������
isr_common_stub:
	pusha                    ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
	mov ax, ds
	push eax                ; �������ݶ�������
	
	mov ax, 0x10            ; �����ں����ݶ���������
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	
	push esp		; ��ʱ�� esp �Ĵ�����ֵ�ȼ��� pt_regs �ṹ���ָ��
	call isr_handler        ; �� C ���Դ�����
	add esp, 4 		; ���ѹ��Ĳ���
	
	pop ebx                 ; �ָ�ԭ�������ݶ�������
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx
	mov ss, bx
	
	popa                     ; Pops edi, esi, ebp, esp, ebx, edx, ecx, eax
	add esp, 8               ; ����ջ��� error code �� ISR
	iret
.end:


; �����ж�����ĺ�
%macro IRQ 2
[GLOBAL irq%1]
irq%1:
	cli
	push 0
	push %2
	jmp irq_common_stub
%endmacro

IRQ   0,    32 	; ����ϵͳ��ʱ��
IRQ   1,    33 	; ����
IRQ   2,    34 	; �� IRQ9 ��ӣ�MPU-401 MD ʹ��
IRQ   3,    35 	; �����豸
IRQ   4,    36 	; �����豸
IRQ   5,    37 	; ��������ʹ��
IRQ   6,    38 	; �����������ʹ��
IRQ   7,    39 	; ��ӡ���������ʹ��
IRQ   8,    40 	; ��ʱʱ��
IRQ   9,    41 	; �� IRQ2 ��ӣ����趨������Ӳ��
IRQ  10,    42 	; ��������ʹ��
IRQ  11,    43 	; ���� AGP �Կ�ʹ��
IRQ  12,    44 	; �� PS/2 ��꣬Ҳ���趨������Ӳ��
IRQ  13,    45 	; Э������ʹ��
IRQ  14,    46 	; IDE0 �������ʹ��
IRQ  15,    47 	; IDE1 �������ʹ��

[GLOBAL irq_common_stub]
[EXTERN irq_handler]
irq_common_stub:
	pusha                    ; pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
	
	mov ax, ds
	push eax                 ; �������ݶ�������
	
	mov ax, 0x10  		 ; �����ں����ݶ�������
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	
	push esp
	call irq_handler
	add esp, 4
	
	pop ebx                   ; �ָ�ԭ�������ݶ�������
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx
	mov ss, bx
	
	popa                     ; Pops edi,esi,ebp...
	add esp, 8     		 ; ����ѹջ�� ������� �� ISR ���
	iret          		 ; ��ջ CS, EIP, EFLAGS, SS, ESP
.end:


