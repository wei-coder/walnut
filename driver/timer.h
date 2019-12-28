
#ifndef __TIMER_H
#define __TIMER_H

#include <types.h>
#include "time.h"

struct tms
{
  time_t tms_utime;		
  time_t tms_stime;		
  time_t tms_cutime;
  time_t tms_cstime;
};


void init_timer(u32 frequency);
void timer_callback();

#endif
