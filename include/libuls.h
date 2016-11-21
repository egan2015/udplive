#ifndef _LIBULS_H_
#define _LIBULS_H_

#include "uls_timer.h"
/* Round an int up to the next multiple of 4.  */
#define WORD_ROUND(s) (((s)+3)&~3)
/* Truncate to the previous multiple of 4.  */
#define WORD_TRUNC(s) ((s)&~3)
#define ULS_VERSION "ULS-0.0.1"

typedef unsigned long uls_atomic_t ;

uls_atomic_t uls_atomic_inc( uls_atomic_t * );
uls_atomic_t uls_atomic_dec_and_test(uls_atomic_t * );
void uls_version_print();
void uls_run_loop();

#endif
