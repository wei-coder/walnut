;filename: page.s
;author:   wei-coder
;date:     2018-2

_SELECTOR_KER_DS	equ	0x68


[GLOBAL page_fault]
[EXTERN do_no_page]
[EXTERN do_wp_page]
page_fault:
	xchg [esp],eax       ;# 取出错码到eax。
	push ecx
	push edx
	push ds
	push es
	push fs
	mov edx,_SELECTOR_KER_DS         ;# 置内核数据段选择符。
	mov ds,dx
	mov es,dx
	mov fs,dx
	mov edx,cr2          ;# 取引起页面异常的线性地址
	push edx              ;# 将该线性地址和出错码压入堆栈，作为调用函数的参数。
	push eax
	test eax,1           ;# 测试标志P，如果不是缺页引起的异常则跳转。
	jne .1
	call do_no_page        ;# 调用缺页处理函数
	jmp .2
.1:	call do_wp_page        ;# 调用写保护处理函数
.2:	add esp,8            ;# 丢弃压入栈的两个参数。
	pop fs
	pop es
	pop ds
	pop edx
	pop ecx
	pop eax
	iret

