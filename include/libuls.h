#ifndef _LIBULS_H_
#define _LIBULS_H_

#include "uls_timer.h"
/* Round an int up to the next multiple of 4.  */
#define WORD_ROUND(s) (((s)+3)&~3)
/* Truncate to the previous multiple of 4.  */
#define WORD_TRUNC(s) ((s)&~3)


void uls_version_print();


#endif
