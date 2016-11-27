#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <pthread.h>

#include "vlstp.h"
#include "structs.h"

#define _SOCKET_POLL_ 0
/**
 * 告诉gcc把这个函数扔到init section
 */
void __vlstp_init(void) __attribute__((constructor));
/**
 * 告诉gcc把这个函数扔到fini section
 */
void __vlstp_fini(void) __attribute__((destructor));


vlstp_atomic_t vlstp_atomic_inc( vlstp_atomic_t * value)
{
	return __sync_add_and_fetch(value, 1);
}

vlstp_atomic_t vlstp_atomic_dec(vlstp_atomic_t * value)
{
	return __sync_sub_and_fetch(value, 1);
}

void __vlstp_init()
{
	printf("vlstp library init \n");
}

void __vlstp_fini()
{
	printf("vlstp library fini\n");
}

void vlstp_dispatch_event()
{
	int ret;
	unsigned long ms = timer_next_msecs( mtime() );
#if _SOCKET_POLL_
	ret = poll(NULL, 0, ms);
#else
	struct timeval tv;
	tv.tv_sec = ( ms / 1000 );
	tv.tv_usec = ( ms % 1000 ) * 1000;
	ret = select(0, NULL, NULL, NULL, &tv);
#endif
	if (ret == 0)
		dispatch_timer();
}
