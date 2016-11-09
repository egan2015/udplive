#ifndef _ULS_TIMER_H
#define _ULS_TIMER_H

#include <sys/time.h>
#include "list.h"

struct uls_timer_list {
	struct list_head list ;
	unsigned long expires;
	unsigned long data;
	void (*function)(unsigned long );
};


// 获取当前时间，毫秒
static inline unsigned long uls_time_now()
{
    struct timeval tv;
    gettimeofday(&tv,0);
    return tv.tv_sec*1000+tv.tv_usec/1000;
}

// 创建建定时器 uls_timer.c
void uls_setup_timer(struct uls_timer_list * timer ,
					unsigned long expires ,
					void (*func)(unsigned long ),
					unsigned long data );

void uls_add_timer(struct uls_timer_list * timer );
void uls_mod_timer(struct uls_timer_list * timer , unsigned long expires );
void uls_del_timer(struct uls_timer_list * timer);


#endif