#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>

#include "vlstp.h"
#include "structs.h"


/**
 * 告诉gcc把这个函数扔到init section
 */
void __vlstp_init(void) __attribute__((constructor));
/**
 * 告诉gcc把这个函数扔到fini section
 */
void __vlstp_fini(void) __attribute__((destructor));


atomic_t vlstp_atomic_inc( atomic_t * value)
{
	return __sync_add_and_fetch(value, 1);
}

atomic_t vlstp_atomic_dec(atomic_t * value)
{
	return __sync_sub_and_fetch(value, 1);
}

void vlstp_version_print()
{
	printf("%s ::%d\n", VLSTP_VERSION, sizeof(rstp_chunkhdr_t));
}

void __vlstp_init()
{
	printf("vlstp library init \n");
}

void __vlstp_fini()
{
	printf("vlstp library fini\n");
}

void vlstp_run()
{
	struct timeval tv;
	int ret;
	unsigned long ms = timer_next_msecs( mtime() );
	tv.tv_sec = ( ms / 1000 );
	tv.tv_usec = ( ms % 1000 ) * 1000;
	ret = select(0, NULL, NULL, NULL, &tv);
	if (ret == 0)
		dispatch_timer();
}
