#ifndef _LIBULS_H_
#define _LIBULS_H_

#include <sys/time.h>
/* Round an int up to the next multiple of 4.  */
#define WORD_ROUND(s) (((s)+3)&~3)
/* Truncate to the previous multiple of 4.  */
#define WORD_TRUNC(s) ((s)&~3)

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

void uls_version_print();


#endif
