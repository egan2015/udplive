#ifndef _STRUCTS_H
#define _STRUCTS_H
#include "list.h"

struct uls_object {
	struct list_head timelist;	
	char name[255];
};

struct uls_object * __uls_object();

#endif