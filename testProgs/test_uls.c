#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "vlstp.h"
#include "bitops.h"
#include "bitmap.h"
struct my_list {
    int id;
    struct list_head list;
    char name[255];
};

#define get_entry( ptr , TYPE ) \
    (TYPE*)(ptr);

struct timer_list timer[4] ;

void timout_event( unsigned long data ) {
    printf(" timer %lu timeout %lu\n", data , mtime());
}

#define LIST_NEW_NODE( ptr , node ) \
    node = malloc( sizeof ( struct list_node)); node->data = ptr;
#define LIST_ADD_NODE( ptr , head ,node ) {\
    LIST_NEW_NODE( ptr,node );  \
    list_add(&node->list,&head); }

static
void test_linux_list_General()
{
    struct mydata {
        int id;
        char name[255];
    };
    struct list_node {
        struct list_head list;
        void * data;
    };
    struct mydata * tmp;
    struct list_head *pos, *q;
    struct list_head head, head1;

    INIT_LIST_HEAD(&head);
    INIT_LIST_HEAD(&head1);
    for ( int i = 0 ; i < 5 ; i++ ) {
        struct mydata * data = malloc( sizeof ( struct mydata) );
        data->id = i;
        sprintf(data->name, "name:%d", i);
        struct list_node * node;
        LIST_ADD_NODE( data, head, node);
        LIST_ADD_NODE( data, head1, node);
    }
    printf("deleting the head using list_for_each_safe\n");

    list_for_each_safe(pos, q, &head) {
        tmp = list_entry(pos, struct list_node, list)->data;
        printf("delete id= %d name = %s \n",
               tmp->id, tmp->name);
        list_del(pos);
        free(list_entry(pos, struct list_node, list));
        //free(tmp);
    }

    printf("deleting the head1 using list_for_each_safe\n");
    list_for_each_safe(pos, q, &head1) {
        tmp = list_entry(pos, struct list_node, list)->data;
        printf("delete id= %d name = %s \n",
               tmp->id, tmp->name);
        list_del(pos);
        free(list_entry(pos, struct list_node, list));
        free(tmp);
    }
    if (list_empty(&head))
        printf("now the head if empty \n");
    if ( list_empty(&head1))
        printf("now the head1 if empty \n");
}

static void print_bit(unsigned long *addr , unsigned long size )
{
    char * buf = malloc ( size );
    char * ptr = buf + size - 1;
    buf[size] = '\0';
    for ( int i = 0 ;  i < size ; i++ , ptr--)
        if (test_bit(i, addr))
            *ptr = 0x31 ;
        else
            *ptr = 0x30;
    printf("%s\n", buf);
}
int main( int argc , char **argv )
{
    //test_linux_list_General();

    unsigned long addr[4], start;
    unsigned long zero_bit;
    memset(addr, 0, sizeof( addr ) );

    set_bit(30, addr);
    set_bit(31, addr);
    set_bit(32, addr);
    set_bit(33, addr);
    set_bit(63, addr);

#if CONFIG_BIT_PRE_LONG==64
    printf("63%s0\n", "<------------------------------------------------------------");
#else
    printf("31%28s0\n", "<----------------------------");
#endif
    for ( int i = 0 ; i < 4; i++)
        print_bit(&addr[i], BITS_PER_LONG);
    printf("bitmap weight :%d\n", bitmap_weight(addr, 128));
    start = find_next_bit(addr, BITS_PER_LONG * 4 , 0 );
    zero_bit = find_first_zero_bit(addr, BITS_PER_LONG * 4);
    printf("next bit %lu,next zero bit %lu first zero_bit %lu\n",
           start, find_next_zero_bit(addr, BITS_PER_LONG * 4, start ),
           zero_bit);

    printf("tsn_le %d tsn_lte %d now %lu\n", TSN_lt(5, 6) , TSN_lte(5, 6) , mtime());

    for ( int i = 0 ; i < 4 ; ++i ) {
        timer[i].expires = mtime() + (i + 1) * 1000 ;
        setup_timer(&timer[i], timout_event, i);
        add_timer(&timer[i]);
    }
    unsigned short seq1 = 65535;
    unsigned short seq2 = seq1 + 1;
    printf("seq1 = %d loss %d  hash and %d\n", seq2,
           (short)(seq2 - seq1), 30 & 31);
    printf("time now %lu next timer %lu\n", mtime(), timer_next_msecs(mtime()));
    printf("mod_timer : %d \n", mod_timer(&timer[3], mtime() + 10000));

    while (1)
        vlstp_dispatch_event();
    return 0;
}
