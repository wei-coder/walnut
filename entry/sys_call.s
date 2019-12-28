;filename: syscall.s
;author:   ����ܹ�������linux0.11��ʵ�֣������ֲ������Լ�ʵ�ֵ�һЩϵͳapi

%if 1

								;system_call.s �ļ�����ϵͳ���� system-call�ײ㴦���ӳ���������Щ����Ƚ����ƣ�����
								;ͬʱҲ����ʱ���жϴ���(timer-interrupt)�����Ӳ�̺����̵��жϴ������Ҳ�����
								;ע�⣺��δ��봦���ź�(signal)ʶ����ÿ��ʱ���жϺ�ϵͳ����֮�󶼻����ʶ��һ��
								;�ж��źŲ��������ź�ʶ����Ϊ���ϵͳ��ɻ��ҡ�
								;��ϵͳ���÷��أ�'ret_from_system_call'��ʱ��ջ�����ݼ�����19-30 �С�

_SELECTOR_USER_DS	equ 0x7B
_SELECTOR_USER_CS	equ	0x73
_SELECTOR_KER_DS	equ 0x68
_SELECTOR_KER_CS	equ	0x60
_SELECTOR_LDT		equ	0x8F

SIG_CHLD         equ	17		;����SIG_CHLD �źţ��ӽ���ֹͣ�������
								;��ջ�и����Ĵ�����ƫ��λ�á�
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
OLDESP           equ	0x28  ;������Ȩ���仯ʱ��
OLDSS            equ	0x2C

								;������Щ������ṹ(task_struct)�б�����ƫ��ֵ���μ�include/linux/sched.h
state		equ	0				; these are offsets into the task-struct.# ����״̬��
counter		equ	4				; ��������ʱ�����(�ݼ�)���δ�����������ʱ��Ƭ��
priority 	equ	8				; ����������������ʼ����ʱcounter=priority��Խ��������ʱ��Խ����
signal		equ	12				; ���ź�λͼ��ÿ������λ����һ���źţ��ź�ֵ=λƫ��ֵ+1��
sigaction 	equ	16				; MUST be 16 (=len of sigaction) sigaction �ṹ���ȱ�����16 �ֽڡ�
                        		; �ź�ִ�����Խṹ�����ƫ��ֵ����Ӧ�źŽ�Ҫִ�еĲ����ͱ�־��Ϣ��
blocked 	equ	(33*16)			; �������ź�λͼ��ƫ������

								; offsets within sigaction
								; ���¶�����sigaction �ṹ�е�ƫ�������μ�include/signal.h
sa_handler 		equ	0          	; �źŴ�����̵ľ��������������
sa_mask 		equ	4           ; �ź���������
sa_flags 		equ	8           ; �źż���
sa_restorer 	equ	12        	; �ָ�����ָ�룬�μ�kernel/signal.c��
nr_system_calls equ	72    		; Linux 0.11 ���ں��е�ϵͳ����������

								; ������ڵ㡣
								; �����ϵͳ���úš�
bad_sys_call:
	mov eax,-1   				; eax ����-1���˳��жϡ�
	iret

[EXTERN schedule]
reschedule:						; ����ִ�е��ȳ�����ڡ����ȳ���schedule ��(kernel/sched.c)��
	push ret_from_sys_call		; ��ret_from_sys_call �ĵ�ַ��ջ
	jmp schedule

[GLOBAL system_call]			; int 0x80 --linux ϵͳ������ڵ�(�����ж�int 0x80��eax ���ǵ��ú�)��
[EXTERN sys_call_table]
[EXTERN current]
[EXTERN task]
[EXTERN do_signal]
system_call:
	cmp eax,nr_system_calls-1   ; ���ú����������Χ�Ļ�����eax ����-1 ���˳���
	ja bad_sys_call
	push ds                		; ����ԭ�μĴ���ֵ��
	push es
	push fs
	push edx              		; ebx,ecx,edx �з���ϵͳ������Ӧ��C ���Ժ����ĵ��ò�����
	push ecx              		; push %ebx,%ecx,%edx as parameters
	push ebx              		; to the system call
	mov edx,_SELECTOR_KER_DS	; set up ds,es to kernel space
	mov ds,dx             		; ds,es ָ���ں����ݶ�, ȫ���������������ݶ���������
	mov es,dx
	mov edx,_SELECTOR_USER_DS	; fs points to local data space
	mov fs,dx             		; fs ָ��ֲ����ݶ�, �ֲ��������������ݶ���������
								; �������������ĺ����ǣ����õ�ַ = _sys_call_table + %eax * 4���μ��б���˵����
								; ��Ӧ��C �����е�sys_call_table ��syscall.h �У����ж�����һ������72 ��
								; ϵͳ����C �������ĵ�ַ�����	
	call [sys_call_table + eax*4]
	push eax                    ; ��ϵͳ���÷���ֵ��ջ��
	mov eax,[current]           ; ȡ��ǰ���񣨽��̣����ݽṹ��ַ->eax��
								; ����鿴��ǰ���������״̬��������ھ���״̬(state ������0)��ȥִ�е��ȳ���
								; ����������ھ���״̬������ʱ��Ƭ�����꣨counter=0������Ҳȥִ�е��ȳ���
	cmp dword [eax+state],0     ; state
	jne reschedule
	cmp dword [eax+counter],0   ; counter
	je reschedule

								; ������δ���ִ�д�ϵͳ����C �������غ󣬶��ź�������ʶ����
[GLOBAL ret_from_sys_call]
ret_from_sys_call:
								; �����б�ǰ�����Ƿ��ǳ�ʼ����task0��������򲻱ض�������ź�������Ĵ���ֱ�ӷ��ء�
								; _task ��ӦC �����е�task[]���飬ֱ������task �൱������task[0]��
	mov eax,[current]			; task[0] cannot have signals
	cmp eax,dword [task]
	je .3
								; ͨ����ԭ���ó������ѡ����ļ�����жϵ��ó����Ƿ����ں�������������1�����������ֱ��
								; �˳��жϡ����������ͨ������������ź����Ĵ�������Ƚ�ѡ����Ƿ�Ϊ��ͨ�û�����ε�ѡ��
								; ��0x000f (RPL=3���ֲ�����1 ����(�����))�������������ת�˳��жϳ���
	cmp word [esp + RCS],_SELECTOR_USER_CS			; was old code segment supervisor ?
	jne .3
								; ���ԭ��ջ��ѡ�����Ϊ0x17��Ҳ��ԭ��ջ�����û����ݶ��У�����Ҳ�˳���
	cmp word [esp + OLDSS],_SELECTOR_USER_DS		; was stack segment = 0x17 ?
	jne .3
								; ������δ������;������ȡ��ǰ����ṹ�е��ź�λͼ(32 λ��ÿλ����1 ���ź�)��
								; Ȼ��������ṹ�е��ź����������Σ��룬������������ź�λ��ȡ����ֵ��С���ź�ֵ���ٰ�
								; ԭ�ź�λͼ�и��źŶ�Ӧ��λ��λ����0������󽫸��ź�ֵ��Ϊ����֮һ����do_signal()��
								; do_signal()�ڣ�kernel/signal.c���У����������13 ����ջ����Ϣ��
	mov ebx,[eax+signal]        ; ȡ�ź�λͼ->ebx��ÿ1 λ����1 ���źţ���32 ���źš�
	mov ecx,[eax+blocked]       ; ȡ���������Σ��ź�λͼ->ecx��
	not ecx                     ; ÿλȡ����
	and ecx,ebx                 ; �����ɵ��ź�λͼ��
	bsf ecx,ecx                 ; �ӵ�λ��λ0����ʼɨ��λͼ�����Ƿ���1 ��λ��
	                            ; ���У���ecx ������λ��ƫ��ֵ�����ڼ�λ0-31����
	je .3
	btr ecx,ebx                 ; ��λ���źţ�ebx ����ԭsignal λͼ����
	mov [eax+signal],ebx        ; ���±���signal λͼ��Ϣ:current->signal��
	inc ecx                     ; ���źŵ���Ϊ��1 ��ʼ����(1-32)��
	push ecx                    ; �ź�ֵ��ջ��Ϊ����do_signal �Ĳ���֮һ��
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

								;### int16 -- ������δ��봦��Э�����������ĳ����źš���תִ��C ����math_error()
								;"kernel/math/math_emulate.c" ���غ���ת��ret_from_sys_call ������ִ�С�
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

								; int7 -- �豸�����ڻ�Э������������[Coprocessor not available]��
								; ������ƼĴ���CR0 ��EM ��־��λ����CPU ִ��һ��ESC ת��ָ��ʱ�ͻ��������жϣ�������
								; �����л���������жϴ������ģ��ESC ת��ָ�
								; CR0 ��TS ��־����CPU ִ������ת��ʱ���õġ�TS ��������ȷ��ʲôʱ��Э�������е����ݣ������ģ�
								; ��CPU ����ִ�е�����ƥ���ˡ���CPU ������һ��ת��ָ��ʱ����TS ��λ�ˣ��ͻ��������жϡ�
								; ��ʱ��Ӧ�ûָ��������Э������ִ��״̬���μ�[kernel/sched.c]�е�˵����
								; ���ж����ת�Ƶ����ret_from_sys_call ��ִ����ȥ����Ⲣ�����źţ���
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
	test eax,0x4				; EM "math emulation bit"; �������EM ������жϣ���ָ�������Э������״̬��
	je math_state_restore       ; ִ��C ����math_state_restore()(kernel/sched.c)��
	push ebp
	push esi
	push edi
	call math_emulate           ; ����C ����math_emulate(kernel/math/math_emulate.c)��
	pop edi
	pop esi
	pop ebp
	ret                         ; �����ret ����ת��ret_from_sys_call��

								; int32 -- (int 0x20) ʱ���жϴ�������ж�Ƶ�ʱ�����Ϊ1000Hz(include/linux/sched.h)��
								; ��ʱоƬ8253/8254 ����(kernel/sched.c)����ʼ���ġ��������jiffies ÿ1 �����1��
								; ��δ��뽫jiffies ��1�����ͽ����ж�ָ���8259 ��������Ȼ���õ�ǰ��Ȩ����Ϊ��������
								; ����do_timer(long CPL)�������÷���ʱתȥ��Ⲣ�����źš�
[GLOBAL timer_interrupt]
[EXTERN jiffies]
[EXTERN do_timer]
timer_interrupt:
	push ds						; ���� ds,es and put kernel data space
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
								; ���ڳ�ʼ���жϿ���оƬʱû�в����Զ�EOI������������Ҫ��ָ�������Ӳ���жϡ�
	mov al,0x20					; EOI to interrupt controller #1
	out 0x20,al          		; ����������OCW2 ��0x20 �˿ڡ�
								; ����3 ���ѡ�����ȡ����ǰ��Ȩ����(0 ��3)��ѹ���ջ����Ϊdo_timer �Ĳ�����
	mov eax,[esp+RCS]
	and eax,3					; %eax is CPL (0 or 3, 0=supervisor)
	push eax
								; do_timer(CPL)ִ�������л�����ʱ�ȹ�������kernel/shched.cʵ�֡�
	call do_timer				; 'do_timer(long CPL)' does everything from
	add esp,4					; task switching to accounting ...
	jmp ret_from_sys_call

								; ����sys_execve()ϵͳ���á�ȡ�жϵ��ó���Ĵ���ָ����Ϊ��������C ����do_execve()��
								; do_execve()��(fs/exec.c)��
[GLOBAL sys_execve]
[EXTERN do_execve]
sys_execve:
								; ��ִ��ϵͳ���õ�ʱ��Ĵ��IPֵ�ĵ�ַ��Ϊ�������ݸ�����do_execve
	lea eax,[esp+REIP]
	push eax
								; ���ú���do_execve�����ʱ���ϵͳջ����ʼ��20���ֽ��д�ŵ���
								; ϵͳ����ʱ��IP�Ĵ�ŵ�ַ������_sys_execveʱ��IPֵ���ļ�����ַ��
								; �������ַ���������ַ
	call do_execve
	add esp,4
	ret

								; sys_fork()���ã����ڴ����ӽ��̣���system_call ����2��ԭ����include/linux/sys.h �С�
								; ���ȵ���C ����find_empty_process()��ȡ��һ�����̺�pid�������ظ�����˵��Ŀǰ��������
								; ������Ȼ�����copy_process()���ƽ��̡�
[GLOBAL sys_fork]
[EXTERN copy_process]
[EXTERN find_empty_process]
sys_fork:
	call find_empty_process     ; ����find_empty_process()(kernel/fork.c)
	test eax,eax
	js .1
	push gs
	push esi
	push edi
	push ebp
	push eax
	call copy_process			; ����C ����copy_process()(kernel/fork.c)
	add esp,20                  ; ������������ѹջ���ݡ�
.1:
	ret

								; int 46 -- (int 0x2E) Ӳ���жϴ��������ӦӲ���ж�����IRQ14��
								; ��Ӳ�̲�����ɻ����ͻᷢ�����ж��źš�(�μ�kernel/blk_drv/hd.c)��
								; ������8259A �жϿ��ƴ�оƬ���ͽ���Ӳ���ж�ָ��(EOI)��Ȼ��ȡ����do_hd �еĺ���ָ�����edx
								; �Ĵ����У�����do_hd ΪNULL�������ж�edx ����ָ���Ƿ�Ϊ�ա����Ϊ�գ����edx ��ֵָ��
								; unexpected_hd_interrupt()��������ʾ������Ϣ�������8259A ��оƬ��EOI ָ�������edx ��
								; ָ��ָ��ĺ���: read_intr()��write_intr()��unexpected_hd_interrupt()��
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
	jmp .2                  	; ��ʱ���á�
.2:
	xor edx,edx
	xchg edx,dword [do_hd]      ; do_hd ����Ϊһ������ָ�룬������ֵread_intr()��
                                ; write_intr()������ַ��(kernel/blk_drv/hd.c)
                                ; �ŵ�edx �Ĵ�����ͽ�do_hd ָ�������ΪNULL��
	test edx,edx         		; ���Ժ���ָ���Ƿ�ΪNull��
	jne .3                  	; ���գ���ʹָ��ָ��C ����unexpected_hd_interrupt()��
	mov edx,unexpected_hd_interrupt     ; (kernel/blk_drv/hdc)
.3:
	out 0x20,al          		; ����8259A �жϿ�����EOI ָ�����Ӳ���жϣ���
	call edx              		; "interesting" way of handling intr.
	pop fs                 		; �Ͼ����do_hd ָ���C ������
	pop es
	pop ds
	pop edx
	pop ecx
	pop eax
	iret

								; int38 -- int 0x26 �����������жϴ��������ӦӲ���ж�����IRQ6��
								; �䴦������������Ӳ�̵Ĵ������һ����(kernel/blk_drv/floppy.c)��
								; ������8259A �жϿ�������оƬ����EOI ָ�Ȼ��ȡ����do_floppy �еĺ���ָ�����eax
								; �Ĵ����У�����do_floppy ΪNULL�������ж�eax ����ָ���Ƿ�Ϊ�ա���Ϊ�գ����eax ��ֵָ��
								; unexpected_floppy_interrupt()��������ʾ������Ϣ��������eax ָ��ĺ���: rw_interrupt,
								; seek_interrupt,recal_interrupt,reset_interrupt ��unexpected_floppy_interrupt��
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

								; int 39 -- (int 0x27) ���п��жϴ�����򣬶�ӦӲ���ж������ź�IRQ7��
								; ���汾�ں˻�δʵ�֡�����ֻ�Ƿ���EOI ָ�
[GLOBAL parallel_interrupt]
parallel_interrupt:
	push eax
	mov al,0x20
	out 0x20,al
	pop eax
	iret


%endif
