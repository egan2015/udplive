#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>

#include "libuls.h"
#include "structs.h"


static struct uls_private_data_t private_data = {
	.name = "uls Instance"
};

void __uls_init(void) __attribute__((constructor)); //告诉gcc把这个函数扔到init section  
void __uls_fini(void) __attribute__((destructor));  //告诉gcc把这个函数扔到fini section

struct uls_private_data_t * __uls_private_data(){

	return &private_data; 
}

void uls_version_print()
{
	printf(" libuls version 1.0.0 \n");
}

void __uls_init()
{
	INIT_LIST_HEAD(&__uls_private_data()->timer_list);
	printf("uls library init \n");
}

void __uls_fini()
{
	printf("uls library fini\n");
}

void uls_run_loop()
{
	struct timeval tv;
	int ret;
	unsigned long ms = get_next_timer_msecs( uls_time_now() );
	tv.tv_sec = ( ms / 1000 );
	tv.tv_usec = ( ms % 1000 ) * 1000;
	ret = select(0,NULL,NULL,NULL,&tv);
	dispatch_timer();
}