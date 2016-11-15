#ifndef _STRUCTS_H
#define _STRUCTS_H
#include "list.h"
#include <stdint.h>

struct uls_private_data_t {
	struct list_head timer_list;	
	char name[255];
};

typedef struct uls_hdr{
	uint32_t	vtag;
	uint32_t	checksum;
}__attribute__((packed))	uls_hdr_t;

typedef struct uls_chunkhdr{
	uint8_t		type;
	uint8_t		flags;
	uint16_t	length;
}__attribute__((packed))	uls_chunkhdr_t;


struct uls_private_data_t * __uls_private_data();

#endif
