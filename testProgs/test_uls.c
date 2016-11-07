#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "libuls.h"


struct my_list{
	int id;
	struct list_head list;
	char name[255];
};

int main( int argc , char **argv )
{
	struct my_list * tmp;
	struct list_head *pos, *q;
	struct my_list myList;
	INIT_LIST_HEAD(&myList.list);
    for ( int i = 0 ; i < 5; i++ ){
    	tmp = (struct my_list*)malloc(sizeof(struct my_list));
    	tmp->id = i+1;
    	sprintf(tmp->name,"list add %d",tmp->id);
    	list_add(&(tmp->list),&myList.list);
    }	
    for( int i = 5 ; i < 7; i++){
    	tmp = (struct my_list*)malloc(sizeof(struct my_list));
    	tmp->id = i+1;
    	sprintf(tmp->name,"list add tail %d",tmp->id);
    	list_add_tail(&(tmp->list),&myList.list);
    }
        printf("head id= %d name = %s\n", 
        list_entry(myList.list.next,struct my_list,list)->id,list_entry(myList.list.next,struct my_list,list)->name); 
    printf("tail id= %d name = %s\n", 
        list_entry(myList.list.prev,struct my_list,list)->id,list_entry(myList.list.prev,struct my_list,list)->name); 

    printf("using list_for_each\n");
    list_for_each(pos,&myList.list){
    	tmp = list_entry(pos,struct my_list, list );
    	printf("id= %d name = %s\n", tmp->id,tmp->name);
    }

    printf("deleting the list using list_for_each_safe\n");
    list_for_each_safe(pos,q,&myList.list){
    	tmp=list_entry(pos,struct my_list,list);
    	printf("delete id= %d name = %s\n", tmp->id,tmp->name);	
    	list_del(pos);
    	free(tmp);
    }
    if(list_empty(&myList.list))
          printf("now the list if empty\n");
	
    uls_version_print();

	printf("WORD_ROUND %d WORD_TRUNC %d \n",WORD_ROUND(3) ,WORD_TRUNC(6) );
	return 0;
}
