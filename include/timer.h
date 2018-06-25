
#ifndef __TIMER_H
#define __TIMER_H

#include "types.h"

typedef long time_t;

struct tms
{
  time_t tms_utime;		
  time_t tms_stime;		
  time_t tms_cutime;
  time_t tms_cstime;
};


void init_timer(u32 frequency);
<<<<<<< HEAD
//void timer_callback(int_cont_t * context);
=======
>>>>>>> 2747b789ab05c6901e2e08b9db7fbb72fd8f6f24
void timer_callback();

#endif
