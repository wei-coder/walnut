<<<<<<< HEAD
/*
filename:	signal.h
author:	wei-coder
*/

#ifndef __SIGNAL_H
#define __SIGNAL_H

struct sigaction {
    void (*sa_handler)(int);    // 信号处理句柄。
    u32 sa_mask;           // 信号的屏蔽码，可以阻塞指定的信号集。
    int sa_flags;               // 信号选项标志。
    void (*sa_restorer)(void);  // 信号恢复函数指针（系统内部使用）。
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
    void (*sa_handler)(int);    // 信号处理句柄。
    u32 sa_mask;           // 信号的屏蔽码，可以阻塞指定的信号集。
    int sa_flags;               // 信号选项标志。
    void (*sa_restorer)(void);  // 信号恢复函数指针（系统内部使用）。
};

#endif
>>>>>>> 2747b789ab05c6901e2e08b9db7fbb72fd8f6f24
