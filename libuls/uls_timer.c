#include "libuls.h"
#include <sys/time.h>
#include <pthread.h>
#include "structs.h"

struct timer_sys
{
	struct list_head timer_list;
	int inited;
}__timer_sys = {
	.inited = 0,
};


static inline 
void detach_timer( struct timer_list * timer )
{
	struct list_head * entry = &timer->entry;
	__list_del(entry->prev,entry->next);
	entry->next	= NULL;
	entry->prev = LIST_POISON2;
}

static inline 
void internal_add_timer(struct list_head * head , struct list_head * new )
{
	struct list_head *pos;
	struct timer_list * ntimer,*ltimer ;
	int done = 0 ;
	ntimer = list_entry(new , struct timer_list , entry );
	list_for_each( pos ,head ) {
		ltimer = list_entry(pos , struct timer_list , entry );
		if ( time_before(ntimer->expires,ltimer->expires) ){
			list_add(new,pos->prev);
			done = 1;
			break;
		}
	}
	if (!done)
		list_add_tail(new , head );
}

void list_all_timer()
{
	struct list_head * pos ;
	struct timer_list * timer ;
	list_for_each( pos , &__timer_sys.timer_list)
	{
		timer = list_entry(pos , struct timer_list , entry );
		printf("expires : %ld\n", timer->expires);
	}
}

void dispatch_timer()
{
	struct timer_list * timer ,*timer1;
	unsigned long now = uls_time_now(); 
	struct list_head * head = &__timer_sys.timer_list;
	list_for_each_entry_safe(timer , timer1 , head , entry ){
		if ( time_after_eq( now , timer->expires)){
			timer->function( timer->data );
			list_del_init(&timer->entry );
		}
	} 
}

unsigned long get_next_timer_msecs( unsigned long now )
{
	unsigned long expires = 1000;
	struct timer_list * head ;
	if (list_empty(&__timer_sys.timer_list))
		return 1000;
	head = list_first_entry(&__timer_sys.timer_list,struct timer_list , entry );

	return head->expires - now;
}	

void setup_timer(struct timer_list * timer ,
					unsigned long expires ,
					void (*func)(unsigned long ),
					unsigned long data )
{
	init_timer(timer);
	timer->expires = expires;
	timer->function = func;
	timer->data = data;
}

void add_timer(struct timer_list * timer)
{
	if ( !__timer_sys.inited )
	{
		INIT_LIST_HEAD(&__timer_sys.timer_list);
		__timer_sys.inited = 1;
	}
	internal_add_timer(&__timer_sys.timer_list,&timer->entry);
}

int mod_timer(struct timer_list * timer  , unsigned long expires )
{
	if ( timer_panding( timer) && timer->expires == expires )
		return 1;
	detach_timer(timer);
	timer->expires = expires;
	internal_add_timer(&__timer_sys.timer_list,&timer->entry);
	return 0;
}

void del_timer( struct timer_list * timer )
{
	if ( timer_panding( timer ))
		detach_timer(timer);
}
