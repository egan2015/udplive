#ifndef _ULS_TIMER_H
#define _ULS_TIMER_H

#include <sys/time.h>

typedef unsigned long timerId ;


// 获取当前时间，毫秒
static inline unsigned long uls_time_now()
{
    struct timeval tv;
    gettimeofday(&tv,0);
    return tv.tv_sec*1000+tv.tv_usec/1000;
}

// 创建建定时器 uls_timer.c
timerId uls_add_timer(unsigned long expires , void (*func)(unsigned long ), unsigned long data );
void	uls_mod_timer(timerId timer , unsigned long expires );
void	uls_del_timer( timerId timer);


#endif