#include <stdio.h>      /* defines printf for tests */
#include <time.h>       /* defines time_t for timings in the test */
#include <stdint.h>     /* defines uint32_t etc */
#include <sys/param.h>  /* attempt to define endianness */
#ifdef linux
# include <endian.h>    /* attempt to define endianness */
#endif

#include "jhash.h"
#include "list.h"

#define HASH_SIZE 64
static struct hlist_head hashtable[HASH_SIZE];

struct tag {
  struct hlist_node  list;
  char name[255];
};

static void hash_add( char * key , struct tag * node )
{
  struct hlist_head *head;
  head = &hashtable[jhash(key, strlen(key), 0) & (HASH_SIZE - 1)];
  hlist_add_head(&node->list, head);
}

struct tag * hash_lookup_tag( const char * key ) {
  struct tag * t;
  struct hlist_head *head;
  struct hlist_node *node ;
  head = &hashtable[jhash(key, strlen(key), 0) & (HASH_SIZE - 1)];
  hlist_for_each_entry(t, node, head, list) {
    if ( !strcmp(t->name, key))
      return t;
  }
  return NULL;
}
void hash_table_destory( struct hlist_head * head )
{
  struct tag * t;
  struct hlist_node * pos , *n;
  hlist_for_each_entry_safe(t, pos, n, head, list) {
    hlist_del_init(pos);
    free(t);
  }
}

void main()
{
  struct tag *tmp;
  char key[255];
  for ( int i = 0 ; i < HASH_SIZE ; ++i)
    INIT_HLIST_HEAD(&hashtable[i]);
  for ( int i = 0 ; i < HASH_SIZE ; ++i) {
    tmp = malloc( sizeof( struct tag ));
    sprintf(tmp->name, "value %d", i);
    hash_add(tmp->name, tmp);
  }
  for ( int i = 0 ; i < HASH_SIZE ; ++i ) {
    sprintf(key, "value %d", i);
    tmp = hash_lookup_tag(key);
    if ( tmp ) {
      printf("key %s hash_lookup_tag value :%s\n", key, tmp->name);
    }
  }
  for ( int i = 0 ; i < HASH_SIZE ; ++i)
    hash_table_destory(&hashtable[i]);
}
