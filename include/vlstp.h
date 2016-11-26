#ifndef __VLSTP_H__
#define __VLSTP_H__

#include <sys/types.h>
#include <stdint.h>
#include "timer.h"
/* Round an int up to the next multiple of 4.  */
#define WORD_ROUND(s) (((s)+3)&~3)
/* Truncate to the previous multiple of 4.  */
#define WORD_TRUNC(s) ((s)&~3)

/* Compare two TSNs */

/* RFC 1982 - Serial Number Arithmetic
 *
 * 2. Comparison
 *  Then, s1 is said to be equal to s2 if and only if i1 is equal to i2,
 *  in all other cases, s1 is not equal to s2.
 *
 * s1 is said to be less than s2 if, and only if, s1 is not equal to s2,
 * and
 *
 *      (i1 < i2 and i2 - i1 < 2^(SERIAL_BITS - 1)) or
 *      (i1 > i2 and i1 - i2 > 2^(SERIAL_BITS - 1))
 *
 * s1 is said to be greater than s2 if, and only if, s1 is not equal to
 * s2, and
 *
 *      (i1 < i2 and i2 - i1 > 2^(SERIAL_BITS - 1)) or
 *      (i1 > i2 and i1 - i2 < 2^(SERIAL_BITS - 1))
 */

/*
 * RFC 2960
 *  1.6 Serial Number Arithmetic
 *
 * Comparisons and arithmetic on TSNs in this document SHOULD use Serial
 * Number Arithmetic as defined in [RFC1982] where SERIAL_BITS = 32.
 */

enum {
  TSN_SIGN_BIT = (1 << 31)
};

static inline int TSN_lt(uint32_t s, uint32_t t)
{
  return ((s) - (t)) & TSN_SIGN_BIT;
}

static inline int TSN_lte(uint32_t s, uint32_t t)
{
  return ((s) == (t)) || (((s) - (t)) & TSN_SIGN_BIT);
}

/* Compare two SSNs */

/*
 * RFC 2960
 *  1.6 Serial Number Arithmetic
 *
 * Comparisons and arithmetic on Stream Sequence Numbers in this document
 * SHOULD use Serial Number Arithmetic as defined in [RFC1982] where
 * SERIAL_BITS = 16.
 */
enum {
  SSN_SIGN_BIT = (1 << 15)
};
/**
 * 判断s小于t
 */
static inline int SSN_lt(uint16_t s, uint16_t t)
{
  return ((s) - (t)) & SSN_SIGN_BIT;
}
/**
 * 判断 s小于等于t
 */
static inline int SSN_lte(uint16_t s, uint16_t t)
{
  return ((s) == (t)) || (((s) - (t)) & SSN_SIGN_BIT);
}

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

