#include <stdio.h>
#include <stdlib.h>
#include "libuls.h"
#include "structs.h"

struct uls_object instance = {
	.name = "uls Instance"
};

struct uls_object * __uls_object(){

	return &instance; 
}

void uls_version_print()
{
	printf(" libuls version 1.0.0 \n");
}

void uls_init()
{
	INIT_LIST_HEAD(&__uls_object()->timers);
}