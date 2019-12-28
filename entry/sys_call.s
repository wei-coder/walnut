;filename: syscall.s
;author:   整体架构借用了linux0.11的实现，后续又补充了自己实现的一些系统api

%if 1

								;system_call.s 文件包含系统调用 system-call底层处理子程序。由于有些代码比较类似，所以
								;同时也包括时钟中断处理(timer-interrupt)句柄。硬盘和软盘的中断处理程序也在这里。
								;注意：这段代码处理信号(signal)识别，在每次时钟中断和系统调用之后都会进行识别。一般
								;中断信号并不处理信号识别，因为会给系统造成混乱。
								;从系统调用返回（'ret_from_system_call'）时堆栈的内容见上面19-30 行。

_SELECTOR_USER_DS	equ 0x7B
_SELECTOR_USER_CS	equ	0x73
_SELECTOR_KER_DS	equ 0x68
_SELECTOR_KER_CS	equ	0x60
_SELECTOR_LDT		equ	0x8F

SIG_CHLD         equ	17		;定义SIG_CHLD 信号（子进程停止或结束）
								;堆栈中各个寄存器的偏移位置。
REAX             equ	0x00
REBX             equ	0x04
RECX             equ	0x08
REDX             equ	0x0C
RFS              equ	0x10
RES              equ	0x14
RDS              equ	0x18
REIP             equ	0x1C
RCS              equ	0x20
EFLAGS           equ	0x24
OLDESP           equ	0x28  ;当有特权级变化时。
OLDSS            equ	0x2C

								;以下这些是任务结构(task_struct)中变量的偏移值，参见include/linux/sched.h
state		equ	0				; these are offsets into the task-struct.# 进程状态码
counter		equ	4				; 任务运行时间计数(递减)（滴答数），运行时间片。
priority 	equ	8				; 运行优先数。任务开始运行时counter=priority，越大则运行时间越长。
signal		equ	12				; 是信号位图，每个比特位代表一种信号，信号值=位偏移值+1。
sigaction 	equ	16				; MUST be 16 (=len of sigaction) sigaction 结构长度必须是16 字节。
                        		; 信号执行属性结构数组的偏移值，对应信号将要执行的操作和标志信息。
blocked 	equ	(33*16)			; 受阻塞信号位图的偏移量。

								; offsets within sigaction
								; 以下定义在sigaction 结构中的偏移量，参见include/signal.h
sa_handler 		equ	0          	; 信号处理过程的句柄（描述符）。
sa_mask 		equ	4           ; 信号量屏蔽码
sa_flags 		equ	8           ; 信号集。
sa_restorer 	equ	12        	; 恢复函数指针，参见kernel/signal.c。
nr_system_calls equ	72    		; Linux 0.11 版内核中的系统调用总数。

								; 定义入口点。
								; 错误的系统调用号。
bad_sys_call:
	mov eax,-1   				; eax 中置-1，退出中断。
	iret

[EXTERN schedule]
reschedule:						; 重新执行调度程序入口。调度程序schedule 在(kernel/sched.c)。
	push ret_from_sys_call		; 将ret_from_sys_call 的地址入栈
	jmp schedule

[GLOBAL system_call]			; int 0x80 --linux 系统调用入口点(调用中断int 0x80，eax 中是调用号)。
[EXTERN sys_call_table]
[EXTERN current]
[EXTERN task]
[EXTERN do_signal]
system_call:
	cmp eax,nr_system_calls-1   ; 调用号如果超出范围的话就在eax 中置-1 并退出。
	ja bad_sys_call
	push ds                		; 保存原段寄存器值。
	push es
	push fs
	push edx              		; ebx,ecx,edx 中放着系统调用相应的C 语言函数的调用参数。
	push ecx              		; push %ebx,%ecx,%edx as parameters
	push ebx              		; to the system call
	mov edx,_SELECTOR_KER_DS	; set up ds,es to kernel space
	mov ds,dx             		; ds,es 指向内核数据段, 全局描述符表中数据段描述符。
	mov es,dx
	mov edx,_SELECTOR_USER_DS	; fs points to local data space
	mov fs,dx             		; fs 指向局部数据段, 局部描述符表中数据段描述符。
								; 下面这句操作数的含义是：调用地址 = _sys_call_table + %eax * 4。参见列表后的说明。
								; 对应的C 程序中的sys_call_table 在syscall.h 中，其中定义了一个包括72 个
								; 系统调用C 处理函数的地址数组表。	
	call [sys_call_table + eax*4]
	push eax                    ; 把系统调用返回值入栈。
	mov eax,[current]           ; 取当前任务（进程）数据结构地址->eax。
								; 下面查看当前任务的运行状态。如果不在就绪状态(state 不等于0)就去执行调度程序。
								; 如果该任务在就绪状态，但其时间片已用完（counter=0），则也去执行调度程序。
	cmp dword [eax+state],0     ; state
	jne reschedule
	cmp dword [eax+counter],0   ; counter
	je reschedule

								; 以下这段代码执行从系统调用C 函数返回后，对信号量进行识别处理。
[GLOBAL ret_from_sys_call]
ret_from_sys_call:
								; 首先判别当前任务是否是初始任务task0，如果是则不必对其进行信号量方面的处理，直接返回。
								; _task 对应C 程序中的task[]数组，直接引用task 相当于引用task[0]。
	mov eax,[current]			; task[0] cannot have signals
	cmp eax,dword [task]
	je .3
								; 通过对原调用程序代码选择符的检查来判断调用程序是否是内核任务（例如任务1）。如果是则直接
								; 退出中断。否则对于普通进程则需进行信号量的处理。这里比较选择符是否为普通用户代码段的选择
								; 符0x000f (RPL=3，局部表，第1 个段(代码段))，如果不是则跳转退出中断程序。
	cmp word [esp + RCS],_SELECTOR_USER_CS			; was old code segment supervisor ?
	jne .3
								; 如果原堆栈段选择符不为0x17（也即原堆栈不在用户数据段中），则也退出。
	cmp word [esp + OLDSS],_SELECTOR_USER_DS		; was stack segment = 0x17 ?
	jne .3
								; 下面这段代码的用途是首先取当前任务结构中的信号位图(32 位，每位代表1 种信号)，
								; 然后用任务结构中的信号阻塞（屏蔽）码，阻塞不允许的信号位，取得数值最小的信号值，再把
								; 原信号位图中该信号对应的位复位（置0），最后将该信号值作为参数之一调用do_signal()。
								; do_signal()在（kernel/signal.c）中，其参数包括13 个入栈的信息。
	mov ebx,[eax+signal]        ; 取信号位图->ebx，每1 位代表1 种信号，共32 个信号。
	mov ecx,[eax+blocked]       ; 取阻塞（屏蔽）信号位图->ecx。
	not ecx                     ; 每位取反。
	and ecx,ebx                 ; 获得许可的信号位图。
	bsf ecx,ecx                 ; 从低位（位0）开始扫描位图，看是否有1 的位，
	                            ; 若有，则ecx 保留该位的偏移值（即第几位0-31）。
	je .3
	btr ecx,ebx                 ; 复位该信号（ebx 含有原signal 位图）。
	mov [eax+signal],ebx        ; 重新保存signal 位图信息:current->signal。
	inc ecx                     ; 将信号调整为从1 开始的数(1-32)。
	push ecx                    ; 信号值入栈作为调用do_signal 的参数之一。
	call do_signal
	pop eax
.3:
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop fs
	pop es
	pop ds
	iret

								;### int16 -- 下面这段代码处理协处理器发出的出错信号。跳转执行C 函数math_error()
								;"kernel/math/math_emulate.c" 返回后将跳转到ret_from_sys_call 处继续执行。
[GLOBAL coprocessor_error]
[EXTERN math_error]
coprocessor_error:
	push ds
	push es
	push fs
	push edx
	push ecx
	push ebx
	push eax
	mov eax,_SELECTOR_KER_DS
	mov ds,ax
	mov es,ax
	mov eax,_SELECTOR_USER_DS
	mov fs,ax
	push ret_from_sys_call
	jmp math_error

								; int7 -- 设备不存在或协处理器不存在[Coprocessor not available]。
								; 如果控制寄存器CR0 的EM 标志置位，则当CPU 执行一个ESC 转义指令时就会引发该中断，这样就
								; 可以有机会让这个中断处理程序模拟ESC 转义指令。
								; CR0 的TS 标志是在CPU 执行任务转换时设置的。TS 可以用来确定什么时候协处理器中的内容（上下文）
								; 与CPU 正在执行的任务不匹配了。当CPU 在运行一个转义指令时发现TS 置位了，就会引发该中断。
								; 此时就应该恢复新任务的协处理器执行状态。参见[kernel/sched.c]中的说明。
								; 该中断最后将转移到标号ret_from_sys_call 处执行下去（检测并处理信号）。
[GLOBAL device_not_available]
[EXTERN math_state_restore]
[EXTERN math_emulate]
device_not_available:
	push ds
	push es
	push fs
	push edx
	push ecx
	push ebx
	push eax
	mov eax,_SELECTOR_KER_DS
	mov ds,ax
	mov es,ax
	mov eax,_SELECTOR_USER_DS
	mov fs,ax
	push ret_from_sys_call
	clts						; clear TS so that we can use math
	mov eax,cr0
	test eax,0x4				; EM "math emulation bit"; 如果不是EM 引起的中断，则恢复新任务协处理器状态，
	je math_state_restore       ; 执行C 函数math_state_restore()(kernel/sched.c)。
	push ebp
	push esi
	push edi
	call math_emulate           ; 调用C 函数math_emulate(kernel/math/math_emulate.c)。
	pop edi
	pop esi
	pop ebp
	ret                         ; 这里的ret 将跳转到ret_from_sys_call。

								; int32 -- (int 0x20) 时钟中断处理程序。中断频率被设置为1000Hz(include/linux/sched.h)，
								; 定时芯片8253/8254 是在(kernel/sched.c)处初始化的。因此这里jiffies 每1 毫秒加1。
								; 这段代码将jiffies 增1，发送结束中断指令给8259 控制器，然后用当前特权级作为参数调用
								; 函数do_timer(long CPL)。当调用返回时转去检测并处理信号。
[GLOBAL timer_interrupt]
[EXTERN jiffies]
[EXTERN do_timer]
timer_interrupt:
	push ds						; 保存 ds,es and put kernel data space
	push es						; into them. %fs is used by _system_call
	push fs
	push edx					; we save %eax,%ecx,%edx as gcc doesn''t
	push ecx					; save those across function calls. %ebx
	push ebx					; is saved as we use that in ret_sys_call
	push eax
	mov eax,_SELECTOR_KER_DS
	mov ds,ax
	mov es,ax
	mov eax,_SELECTOR_USER_DS
	mov fs,ax
	inc dword [jiffies]
								; 由于初始化中断控制芯片时没有采用自动EOI，所以这里需要发指令结束该硬件中断。
	mov al,0x20					; EOI to interrupt controller #1
	out 0x20,al          		; 操作命令字OCW2 送0x20 端口。
								; 下面3 句从选择符中取出当前特权级别(0 或3)并压入堆栈，作为do_timer 的参数。
	mov eax,[esp+RCS]
	and eax,3					; %eax is CPL (0 or 3, 0=supervisor)
	push eax
								; do_timer(CPL)执行任务切换、计时等工作，在kernel/shched.c实现。
	call do_timer				; 'do_timer(long CPL)' does everything from
	add esp,4					; task switching to accounting ...
	jmp ret_from_sys_call

								; 这是sys_execve()系统调用。取中断调用程序的代码指针作为参数调用C 函数do_execve()。
								; do_execve()在(fs/exec.c)。
[GLOBAL sys_execve]
[EXTERN do_execve]
sys_execve:
								; 将执行系统调用的时候的存放IP值的地址作为参数传递给函数do_execve
	lea eax,[esp+REIP]
	push eax
								; 调用函数do_execve。这个时候从系统栈顶开始的20个字节中存放的是
								; 系统调用时的IP的存放地址、调用_sys_execve时的IP值、文件名地址、
								; 参数表地址、环境表地址
	call do_execve
	add esp,4
	ret

								; sys_fork()调用，用于创建子进程，是system_call 功能2。原形在include/linux/sys.h 中。
								; 首先调用C 函数find_empty_process()，取得一个进程号pid。若返回负数则说明目前任务数组
								; 已满。然后调用copy_process()复制进程。
[GLOBAL sys_fork]
[EXTERN copy_process]
[EXTERN find_empty_process]
sys_fork:
	call find_empty_process     ; 调用find_empty_process()(kernel/fork.c)
	test eax,eax
	js .1
	push gs
	push esi
	push edi
	push ebp
	push eax
	call copy_process			; 调用C 函数copy_process()(kernel/fork.c)
	add esp,20                  ; 丢弃这里所有压栈内容。
.1:
	ret

								; int 46 -- (int 0x2E) 硬盘中断处理程序，响应硬件中断请求IRQ14。
								; 当硬盘操作完成或出错就会发出此中断信号。(参见kernel/blk_drv/hd.c)。
								; 首先向8259A 中断控制从芯片发送结束硬件中断指令(EOI)，然后取变量do_hd 中的函数指针放入edx
								; 寄存器中，并置do_hd 为NULL，接着判断edx 函数指针是否为空。如果为空，则给edx 赋值指向
								; unexpected_hd_interrupt()，用于显示出错信息。随后向8259A 主芯片送EOI 指令，并调用edx 中
								; 指针指向的函数: read_intr()、write_intr()或unexpected_hd_interrupt()。
[GLOBAL hd_interrupt]
[EXTERN do_hd]
[EXTERN unexpected_hd_interrupt]
hd_interrupt:
	push eax
	push ecx
	push edx
	push ds
	push es
	push fs
	mov eax,_SELECTOR_KER_DS
	mov ds,ax
	mov es,ax
	mov eax,_SELECTOR_USER_DS
	mov fs,ax
	mov al,0x20
	out 0xA0,al					; EOI to interrupt controller 1
	jmp .1						; give port chance to breathe
.1:
	jmp .2                  	; 延时作用。
.2:
	xor edx,edx
	xchg edx,dword [do_hd]      ; do_hd 定义为一个函数指针，将被赋值read_intr()或
                                ; write_intr()函数地址。(kernel/blk_drv/hd.c)
                                ; 放到edx 寄存器后就将do_hd 指针变量置为NULL。
	test edx,edx         		; 测试函数指针是否为Null。
	jne .3                  	; 若空，则使指针指向C 函数unexpected_hd_interrupt()。
	mov edx,unexpected_hd_interrupt     ; (kernel/blk_drv/hdc)
.3:
	out 0x20,al          		; 送主8259A 中断控制器EOI 指令（结束硬件中断）。
	call edx              		; "interesting" way of handling intr.
	pop fs                 		; 上句调用do_hd 指向的C 函数。
	pop es
	pop ds
	pop edx
	pop ecx
	pop eax
	iret

								; int38 -- int 0x26 软盘驱动器中断处理程序，响应硬件中断请求IRQ6。
								; 其处理过程与上面对硬盘的处理基本一样。(kernel/blk_drv/floppy.c)。
								; 首先向8259A 中断控制器主芯片发送EOI 指令，然后取变量do_floppy 中的函数指针放入eax
								; 寄存器中，并置do_floppy 为NULL，接着判断eax 函数指针是否为空。如为空，则给eax 赋值指向
								; unexpected_floppy_interrupt()，用于显示出错信息。随后调用eax 指向的函数: rw_interrupt,
								; seek_interrupt,recal_interrupt,reset_interrupt 或unexpected_floppy_interrupt。
[GLOBAL floppy_interrupt]
[EXTERN do_floppy]
[EXTERN unexpected_floppy_interrupt]
floppy_interrupt:
	push eax
	push ecx
	push edx
	push ds
	push es
	push fs
	mov eax,_SELECTOR_KER_DS
	mov ds,ax
	mov es,ax
	mov eax,_SELECTOR_USER_DS
	mov fs,ax
	mov al,0x20
	out 0x20,al					; EOI to interrupt controller ;1
	xor eax,eax
	xchg eax,dword [do_floppy]
	test eax,eax
	jne .1
	mov eax,unexpected_floppy_interrupt
.1:
	call eax					; "interesting" way of handling intr.
	pop fs
	pop es
	pop ds
	pop edx
	pop ecx
	pop eax
	iret

								; int 39 -- (int 0x27) 并行口中断处理程序，对应硬件中断请求信号IRQ7。
								; 本版本内核还未实现。这里只是发送EOI 指令。
[GLOBAL parallel_interrupt]
parallel_interrupt:
	push eax
	mov al,0x20
	out 0x20,al
	pop eax
	iret


%endif
