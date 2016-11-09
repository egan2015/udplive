#include "libuls.h"
#include <sys/time.h>
#include <pthread.h>
#include "list.h"
#include "structs.h"

struct uls_timer_t {
	struct list_head list ;
	unsigned long expires;
	unsigned long data;
	void (*func)(unsigned long );
};

// 加入定时器
static void uls_timer_add(struct uls_timer_t * timer);
//检查定时器是否执行
static int  uls_timer_panding(struct uls_timer_t * timer );
//修改定时器
static int  uls_timer_reset(struct uls_timer_t * timer );
//删除定时器
static void uls_timer_del(struct uls_timer_t * timer );

timerId uls_add_timer(unsigned long expires ,
						 void (*func)(unsigned long ),
						  unsigned long data )
{
	printf("%s\n", __uls_object()->name);
	return 0;
}
void uls_mod_timer(timerId timer , unsigned long expires )
{

}

void uls_del_timer( timerId timer)
{

}
