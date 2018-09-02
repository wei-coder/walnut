
#ifndef __TRAP_GATE_H
#define __TRAP_GATE_H

int do_exit(long code);                 // �����˳�����(kernel/exit.c)
void page_exception(void);          // ҳ�쳣��ʵ����page_fault (mm/page.s)
// ���¶�����һЩ�жϴ������ԭ�ͣ������ڣ�kernel/asm.s ��system_call.s���С�
void divide_error(void);              // int0 (kernel/asm.s)
void debug(void);
void nmi(void);
void int3(void);
void overflow(void);
void bounds(void);
void invalid_op(void);                // int6 (kernel/asm.s)
void device_not_available(void);   // int7 (kernel/system_call.s)
void double_fault(void);             // int8 (kernel/asm.s)
void coprocessor_segment_overrun(void);
void invalid_TSS(void);
void segment_not_present(void);
void stack_segment(void);
void general_protection(void);
void page_fault(void);              // int14 (mm/page.s)
void coprocessor_error(void);
void reserved(void);
void parallel_interrupt(void);
void irq13(void);                     // int45 Э�������жϴ���(kernel/asm.s)
#endif