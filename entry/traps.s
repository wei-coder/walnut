;filename: traps.s
;author:   wei-coder
;date:	   2018-2


_SELECTOR_KER_DS	equ	0x68


;# int0
;# �����Ǳ��������(divide_error)������롣���'_divide_error'ʵ������C ���Ժ�
;# ��divide_error()�����������ģ���ж�Ӧ�����ơ�'_do_divide_error'������traps.c �С�
[GLOBAL divide_error]
[EXTERN do_divide_error]
divide_error:
	push do_divide_error
no_error_code:
	xchg [esp],eax
	push ebx
	push ecx
	push edx
	push edi
	push esi
	push ebp
	push ds
	push es
	push fs
	push 0			;# "error code"
	lea edx,[esp+44]       ;# ȡԭ���÷��ص�ַ����ջָ��λ�ã���ѹ���ջ��
	push edx
	mov edx,_SELECTOR_KER_DS         ;# �ں˴������ݶ�ѡ�����
	mov ds,dx
	mov es,dx
	mov fs,dx             ;# �����ϵ�'*'�ű�ʾ�Ǿ��Ե��ò������������ָ��PC �޹ء�
	call eax              ;# ����C ����do_divide_error()��
	add esp,8            ;# �ö�ջָ������ָ��Ĵ���fs ��ջ����
	pop fs
	pop es
	pop ds
	pop ebp
	pop esi
	pop edi
	pop edx
	pop ecx
	pop ebx
	pop eax
	iret

;# int1 -- debug �����ж���ڵ㡣�������ͬ�ϡ�
[GLOBAL debug]
[EXTERN do_int3]
debug:
	push do_int3		;# _do_debug # _do_debug C ����ָ����ջ������ͬ��
	jmp no_error_code
;# int2 -- �������жϵ�����ڵ㡣
[GLOBAL nmi]
[EXTERN do_nmi]
nmi:
	push do_nmi
	jmp no_error_code

[GLOBAL int3]
[EXTERN do_int3]
int3:
	push do_int3
	jmp no_error_code
;# int4 -- ����������ж���ڵ㡣
[GLOBAL overflow]
[EXTERN do_overflow]
overflow:
	push do_overflow
	jmp no_error_code
;# int5 -- �߽�������ж���ڵ㡣
[GLOBAL bounds]
[EXTERN do_bounds]
bounds:
	push do_bounds
	jmp no_error_code
;# int6 -- ��Ч����ָ������ж���ڵ㡣
[GLOBAL invalid_op]
[EXTERN do_invalid_op]
invalid_op:
	push do_invalid_op
	jmp no_error_code
;# int9 -- Э�������γ��������ж���ڵ㡣
[GLOBAL coprocessor_segment_overrun]
[EXTERN do_coprocessor_segment_overrun]
coprocessor_segment_overrun:
	push do_coprocessor_segment_overrun
	jmp no_error_code
;# int15 �C ������
[GLOBAL reserved]
[EXTERN do_reserved]
reserved:
	push do_reserved
	jmp no_error_code
	
;# int45 -- ( = 0x20 + 13 ) ��ѧЭ��������Coprocessor���������жϡ�
;# ��Э������ִ����һ������ʱ�ͻᷢ��IRQ13 �ж��źţ���֪ͨCPU ������ɡ�
[GLOBAL irq13]
[EXTERN coprocessor_error]
irq13:
	push eax
	xor al,al            ;# 80387 ��ִ�м���ʱ��CPU ��ȴ����������ɡ�
	out 0xF0,al          ;# ͨ��д0xF0 �˿ڣ����жϽ�����CPU ��BUSY �����źţ�������
                                ;# ����80387 �Ĵ�������չ��������PEREQ���ò�����Ҫ��Ϊ��ȷ��
                                ;# �ڼ���ִ��80387 ���κ�ָ��֮ǰ����Ӧ���жϡ�
	mov al,0x20
	out 0x20,al          ;# ��8259 ���жϿ���оƬ����EOI���жϽ������źš�
	jmp .1
.1:	jmp .2
.2:	out 0xA0,al          ;# ����8259 ���жϿ���оƬ����EOI���жϽ������źš�
	pop eax
	jmp coprocessor_error  ;# _coprocessor_error ԭ���ڱ��ļ��У������Ѿ��ŵ�
                                ;# ��kernel/system_call.s��

;# �����ж��ڵ���ʱ�����жϷ��ص�ַ֮�󽫳����ѹ���ջ����˷���ʱҲ��Ҫ������ŵ�����
;# int8 -- ˫������ϡ�
[GLOBAL double_fault]
[EXTERN do_double_fault]
double_fault:
	push do_double_fault
error_code:
	xchg [esp+4],eax		;# error code <-> %eax
	xchg [esp],ebx		;# &function <-> %ebx
	push ecx
	push edx
	push edi
	push esi
	push ebp
	push ds
	push es
	push fs
	push eax			;# error code
	lea eax,[esp+44]		;# offset
	push eax
	mov eax,_SELECTOR_KER_DS
	mov ds,ax
	mov es,ax
	mov fs,ax
	call ebx
	add esp,8
	pop fs
	pop es
	pop ds
	pop ebp
	pop esi
	pop edi
	pop edx
	pop ecx
	pop ebx
	pop eax
	iret
;# int10 -- ��Ч������״̬��(TSS)��
[GLOBAL invalid_TSS]
[EXTERN do_invalid_TSS]
invalid_TSS:
	push do_invalid_TSS
	jmp error_code
;# int11 -- �β����ڡ�
[GLOBAL segment_not_present]
[EXTERN do_segment_not_present]
segment_not_present:
	push do_segment_not_present
	jmp error_code
;# int12 -- ��ջ�δ���
[GLOBAL stack_segment]
[EXTERN do_stack_segment]
stack_segment:
	push do_stack_segment
	jmp error_code
;# int13 -- һ�㱣���Գ���
[GLOBAL general_protection]
[EXTERN do_general_protection]
general_protection:
	push do_general_protection
	jmp error_code

;# int7 -- �豸������(_device_not_available)��(kernel/system_call.s)
;# int14 -- ҳ����(_page_fault)��(mm/page.s,14)
;# int16 -- Э����������(_coprocessor_error)��(kernel/system_call.s)
;# ʱ���ж�int 0x20 (_timer_interrupt)��(kernel/system_call.s)
;# ϵͳ����int 0x80 (_system_call)�ڣ�kernel/system_call.s��

