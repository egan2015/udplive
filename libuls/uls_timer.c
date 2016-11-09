#include "libuls.h"
#include <sys/time.h>
#include <pthread.h>
#include "structs.h"


void uls_setup_timer(struct uls_timer_list * timer ,
					unsigned long expires ,
					void (*func)(unsigned long ),
					unsigned long data )
{
	INIT_LIST_HEAD(&timer->list);
	timer->expires = expires;
	timer->function = func;
	timer->data = data;
}

void uls_add_timer(struct uls_timer_list * timer)
{
	printf("%s\n", __uls_object()->name);
	list_add_tail(&timer->list,&__uls_object()->timers);
}
void uls_mod_timer(struct uls_timer_list * timer  , unsigned long expires )
{

}

void uls_del_timer( struct uls_timer_list * timer )
{

}
