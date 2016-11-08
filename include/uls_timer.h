#ifndef _ULS_TIMER_H_
#define _ULS_TIMER_H_


struct uls_timer_t ;

int uls_add_timer(struct uls_timer_t * timer , unsigned long expires 
				,unsigned long data , void (*callback)(unsigned long));


#endif