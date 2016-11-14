#ifndef _STRUCTS_H
#define _STRUCTS_H
#include "list.h"

struct uls_private_data_t {
	struct list_head timer_list;	
	char name[255];
};

struct uls_private_data_t * __uls_private_data();

#endif