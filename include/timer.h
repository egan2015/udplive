#ifndef _ULS_TIMER_H
#define _ULS_TIMER_H

#include <sys/time.h>
#include "list.h"

struct timer_list {
	struct list_head entry ;
	unsigned long expires;
	void (*function)(unsigned long );
	unsigned long data;
};

#define time_after(a,b) ((long)(b) - (long)(a) < 0)
#define time_before(a,b) time_after(b,a)

#define time_after_eq(a,b) ((long)(a) - (long)(b) >= 0)
#define time_before_eq(a,b) time_after_eq(b,a)

/* 获取当前时间，毫秒*/
static	inline	unsigned long mtime()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static inline	void init_timer(struct timer_list * timer)
{
	timer->entry.next = timer->entry.prev = NULL;
}

static inline	int timer_panding( const struct timer_list * timer ) {
	return timer->entry.next != NULL;
}
/* 创建建定时器 uls_timer.c */
void setup_timer(struct timer_list * timer ,
                 void (*func)(unsigned long ),
                 unsigned long data );

void add_timer(struct timer_list * timer );
int  mod_timer(struct timer_list * timer , unsigned long expires );
void del_timer(struct timer_list * timer);

unsigned long timer_next_msecs( unsigned long now );
void dispatch_timer();

#endif