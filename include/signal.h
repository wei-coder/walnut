<<<<<<< HEAD
/*
filename:	signal.h
author:	wei-coder
*/

#ifndef __SIGNAL_H
#define __SIGNAL_H

struct sigaction {
    void (*sa_handler)(int);    // �źŴ�������
    u32 sa_mask;           // �źŵ������룬��������ָ�����źż���
    int sa_flags;               // �ź�ѡ���־��
    void (*sa_restorer)(void);  // �źŻָ�����ָ�루ϵͳ�ڲ�ʹ�ã���
};

#endif
=======
/*
filename:	signal.h
author:	wei-coder
*/

#ifndef __SIGNAL_H
#define __SIGNAL_H

struct sigaction {
    void (*sa_handler)(int);    // �źŴ�������
    u32 sa_mask;           // �źŵ������룬��������ָ�����źż���
    int sa_flags;               // �ź�ѡ���־��
    void (*sa_restorer)(void);  // �źŻָ�����ָ�루ϵͳ�ڲ�ʹ�ã���
};

#endif
>>>>>>> 2747b789ab05c6901e2e08b9db7fbb72fd8f6f24
