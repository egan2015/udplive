#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

static int count = 0;

void *test_func(void *arg)
{
	int i=0;
	for(i=0;i<20000;++i){
			__sync_fetch_and_add(&count,1);
	}
	return NULL;
}

typedef struct my_struct{
	uint8_t		type1;
	uint16_t	type2;
	uint32_t	type3;
} __attribute__((packed)) mydata;

int main(int argc, const char *argv[])
{
	pthread_t id[20];
	int i = 0;

	for(i=0;i<20;++i){
			pthread_create(&id[i],NULL,test_func,NULL);
	}

	for(i=0;i<20;++i){
			pthread_join(id[i],NULL);
	}

	printf("%d :: %d\n",count,sizeof(mydata));
	return 0;
}
