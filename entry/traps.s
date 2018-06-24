;filename: traps.s
;author:   wei-coder
;date:	   2018-2


_SELECTOR_KER_DS	equ	0x68


;# int0
;# 下面是被零除出错(divide_error)处理代码。标号'_divide_error'实际上是C 语言函
;# 数divide_error()编译后所生成模块中对应的名称。'_do_divide_error'函数在traps.c 中。
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
	lea edx,[esp+44]       ;# 取原调用返回地址处堆栈指针位置，并压入堆栈。
	push edx
	mov edx,_SELECTOR_KER_DS         ;# 内核代码数据段选择符。
	mov ds,dx
	mov es,dx
	mov fs,dx             ;# 下行上的'*'号表示是绝对调用操作数，与程序指针PC 无关。
	call eax              ;# 调用C 函数do_divide_error()。
	add esp,8            ;# 让堆栈指针重新指向寄存器fs 入栈处。
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

;# int1 -- debug 调试中断入口点。处理过程同上。
[GLOBAL debug]
[EXTERN do_int3]
debug:
	push do_int3		;# _do_debug # _do_debug C 函数指针入栈。以下同。
	jmp no_error_code
;# int2 -- 非屏蔽中断调用入口点。
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
;# int4 -- 溢出出错处理中断入口点。
[GLOBAL overflow]
[EXTERN do_overflow]
overflow:
	push do_overflow
	jmp no_error_code
;# int5 -- 边界检查出错中断入口点。
[GLOBAL bounds]
[EXTERN do_bounds]
bounds:
	push do_bounds
	jmp no_error_code
;# int6 -- 无效操作指令出错中断入口点。
[GLOBAL invalid_op]
[EXTERN do_invalid_op]
invalid_op:
	push do_invalid_op
	jmp no_error_code
;# int9 -- 协处理器段超出出错中断入口点。
[GLOBAL coprocessor_segment_overrun]
[EXTERN do_coprocessor_segment_overrun]
coprocessor_segment_overrun:
	push do_coprocessor_segment_overrun
	jmp no_error_code
;# int15 – 保留。
[GLOBAL reserved]
[EXTERN do_reserved]
reserved:
	push do_reserved
	jmp no_error_code
	
;# int45 -- ( = 0x20 + 13 ) 数学协处理器（Coprocessor）发出的中断。
;# 当协处理器执行完一个操作时就会发出IRQ13 中断信号，以通知CPU 操作完成。
[GLOBAL irq13]
[EXTERN coprocessor_error]
irq13:
	push eax
	xor al,al            ;# 80387 在执行计算时，CPU 会等待其操作的完成。
	out 0xF0,al          ;# 通过写0xF0 端口，本中断将消除CPU 的BUSY 延续信号，并重新
                                ;# 激活80387 的处理器扩展请求引脚PEREQ。该操作主要是为了确保
                                ;# 在继续执行80387 的任何指令之前，响应本中断。
	mov al,0x20
	out 0x20,al          ;# 向8259 主中断控制芯片发送EOI（中断结束）信号。
	jmp .1
.1:	jmp .2
.2:	out 0xA0,al          ;# 再向8259 从中断控制芯片发送EOI（中断结束）信号。
	pop eax
	jmp coprocessor_error  ;# _coprocessor_error 原来在本文件中，现在已经放到
                                ;# （kernel/system_call.s）

;# 以下中断在调用时会在中断返回地址之后将出错号压入堆栈，因此返回时也需要将出错号弹出。
;# int8 -- 双出错故障。
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
;# int10 -- 无效的任务状态段(TSS)。
[GLOBAL invalid_TSS]
[EXTERN do_invalid_TSS]
invalid_TSS:
	push do_invalid_TSS
	jmp error_code
;# int11 -- 段不存在。
[GLOBAL segment_not_present]
[EXTERN do_segment_not_present]
segment_not_present:
	push do_segment_not_present
	jmp error_code
;# int12 -- 堆栈段错误。
[GLOBAL stack_segment]
[EXTERN do_stack_segment]
stack_segment:
	push do_stack_segment
	jmp error_code
;# int13 -- 一般保护性出错。
[GLOBAL general_protection]
[EXTERN do_general_protection]
general_protection:
	push do_general_protection
	jmp error_code

;# int7 -- 设备不存在(_device_not_available)在(kernel/system_call.s)
;# int14 -- 页错误(_page_fault)在(mm/page.s,14)
;# int16 -- 协处理器错误(_coprocessor_error)在(kernel/system_call.s)
;# 时钟中断int 0x20 (_timer_interrupt)在(kernel/system_call.s)
;# 系统调用int 0x80 (_system_call)在（kernel/system_call.s）

