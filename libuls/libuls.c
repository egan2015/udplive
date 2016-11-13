#include <stdio.h>
#include <stdlib.h>
#include "libuls.h"
#include "structs.h"

static struct uls_private_data_t private_data = {
	.name = "uls Instance"
};

struct uls_private_data_t * __uls_private_data(){

	return &private_data; 
}

void uls_version_print()
{
	printf(" libuls version 1.0.0 \n");
}

void uls_init()
{
	INIT_LIST_HEAD(&__uls_private_data()->timers);
}