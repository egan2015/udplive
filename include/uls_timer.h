#ifndef _ULS_TIMER_H_
#define _ULS_TIMER_H_

#include <sys/time.h>
#include "list.h"

struct uls_timer_t {
	struct list_head list ;
	unsigned long expires;
	unsigned long data;
	void (*func)(unsigned long );
};

// 获取当前时间，毫秒
static inline unsigned long uls_time()
{
    struct timeval tv;
    gettimeofday(&tv,0);
    return tv.tv_sec*1000+tv.tv_usec/1000;
}

// 加入定时器
void uls_timer_add(struct uls_timer_t * timer);

//检查定时器是否执行
int uls_timer_panding(struct uls_timer_t * timer );

//修改定时器
int uls_timer_reset(struct uls_timer_t * timer );

//删除定时器
void uls_timer_del(struct uls_timer_t * timer );

#endif