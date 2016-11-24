#ifndef __VLSTP_H__
#define __VLSTP_H__

#include "timer.h"
/* Round an int up to the next multiple of 4.  */
#define WORD_ROUND(s) (((s)+3)&~3)
/* Truncate to the previous multiple of 4.  */
#define WORD_TRUNC(s) ((s)&~3)
#define VLSTP_VERSION "VLSTP-0.0.1"

typedef unsigned long atomic_t ;

atomic_t vlstp_atomic_inc( atomic_t * );
atomic_t vlstp_atomic_dec(atomic_t * );

void vlstp_version_print();
void vlstp_run();

#endif

