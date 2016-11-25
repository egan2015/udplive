#ifndef __VLSTP_H__
#define __VLSTP_H__

#include "timer.h"
/* Round an int up to the next multiple of 4.  */
#define WORD_ROUND(s) (((s)+3)&~3)
/* Truncate to the previous multiple of 4.  */
#define WORD_TRUNC(s) ((s)&~3)

typedef unsigned long vlstp_atomic_t ;
/**
 * 初始化 vlstp ,并创建对象实例
 * return 成功 0 ， 失败 -1;
 */
int vlstp_initialize();

/* 原子操作，用于对象引用计数器 */
vlstp_atomic_t vlstp_atomic_inc(vlstp_atomic_t * );
vlstp_atomic_t vlstp_atomic_dec(vlstp_atomic_t * );

/* 执行系统调度，分配系统事件*/
void vlstp_dispatch_event();

#endif

