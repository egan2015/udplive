#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "vlstp.h"
#include "bitops.h"
#include "bitmap.h"

#define LOGI(...) printf(__VA_ARGS__);

struct my_list {
    int id;
    struct list_head list;
    char name[255];
};

struct objbase
{
    vlstp_atomic_t refcnt;
    void (*destroy)(void *);
};

static void objbase_init( struct objbase *obj , void (*destroy)(struct objbase *))
{
    obj->destroy = destroy;
    obj->refcnt = 1;
}

static void objbase_addref( struct objbase * obj )
{
    vlstp_atomic_inc(&obj->refcnt);
}
static void objbase_release( struct objbase *obj)
{
    if ( vlstp_atomic_dec(&obj->refcnt) <= 0) {
        obj->destroy(obj);
    }
}

struct typedobj
{
    struct objbase base;
    char *buf;
};

static void typedobj_destroy( struct typedobj * obj )
{
    LOGI("destroy %s\n", obj->buf);
    if (obj->buf)
        free( obj->buf );
    obj->buf = NULL;
    free(obj);
    obj = NULL;
}

static struct typedobj * typedobj_new()
{
    struct typedobj *obj = malloc( sizeof( struct typedobj));
    objbase_init(&obj->base, typedobj_destroy);
    obj->buf = malloc( 100 );
    strcpy(obj->buf, "typedobj obj");
    return obj;
}


#define get_entry( ptr , TYPE ) \
    (TYPE*)(ptr);

struct timer_list timer[4] ;

void timout_event( unsigned long data ) {
    LOGI(" timer %lu timeout %lu\n", data , mtime());
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
struct node {
    struct node *next;
    int value;
};

static void link_test1()
{
    struct node * head = malloc( sizeof(struct node ));
    head->value = 20;
    head->next = NULL;
    for ( int i = 0; i < 20 ; ++i) {
        struct node **ppev = &head;
        struct node *current , *new;
        while ((current = *ppev) != NULL && current->value < i)
            ppev = &current->next;
        new = malloc(sizeof(struct node ));
        new->value = i;
        new->next = current;
        *ppev = new;
    }
    /*print list and delete */
    while ( head != NULL ) {
        struct node * next = head->next;
        printf("node value :%d\n", head->value );
        free(head);
        head = next;
    }
}

static void list_test2()
{
    struct node * head = malloc( sizeof(struct node ));
    head->value = 20;
    head->next = NULL;
    for ( int i = 0; i < 20 ; ++i) {
        struct node *ppev = NULL;
        struct node *current = head, *new;
        while (current != NULL && current->value < i) {
            ppev = current;
            current = current->next;
        }
        new = malloc(sizeof(struct node ));
        new->value = i;
        new->next = current;
        if ( ppev == NULL )
            head = new;
        else
            ppev->next = new;
    }
    /*print list and delete */
    while ( head != NULL ) {
        struct node * next = head->next;
        printf("list 2 node value :%d\n", head->value );
        free(head);
        head = next;
    }
}

static void print_bit(unsigned long * addr , unsigned long size )
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
    free(buf);
}

static void pointer_test()
{
    int i = 5;
    int *pi = &i;
    int **ppi = &pi;
    printf("i = %d , pi = %d , ppi = %d\n", i , *pi , **ppi  );

    *pi = i + 5;
    printf("i = %d , pi = %d , ppi = %d\n", i , *pi , **ppi  );
    **ppi = i + 5;
    printf("i = %d , pi = %d , ppi = %d\n", i , *pi , **ppi  );

}

int main( int argc , char **argv )
{
    //test_linux_list_General();
    struct typedobj *obj = NULL;
    unsigned long addr[4], start;
    unsigned long zero_bit;
    memset(addr, 0, sizeof( addr ) );

    link_test1();
    list_test2();
    pointer_test();

    set_bit(60, addr);
    set_bit(61, addr);
    set_bit(62, addr);
    set_bit(63, addr);
    set_bit(64, addr);

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

    LOGI("create typedobj obj \n");
    obj = typedobj_new();
    objbase_addref(obj);
    LOGI("obj name :%s , refcnt %ld\n", obj->buf, obj->base.refcnt);
    objbase_release(obj);
    objbase_release(obj);
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
