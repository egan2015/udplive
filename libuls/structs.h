#ifndef _ulstp_structs_h
#define _rstp_structs_h

#include "list.h"
#include <stdint.h>

struct ulstp_private_data_t {
	struct list_head timer_list;
	char name[255];
};

typedef
struct ulstp_hdr {
	uint16_t	source;		/*source port*/
	uint16_t	dest;		/*dest port*/
	uint32_t	vtag;
	uint32_t	checksum;
} __attribute__((packed))	ulstp_hdr_t;

typedef
struct ulstp_chunkhdr {
	uint8_t		type;
	uint8_t		flags;
	uint16_t	length;
} __attribute__((packed))	ulstp_chunkhdr_t;

/* Section 3.2.  Chunk Type Values.
 * [Chunk Type] identifies the type of information contained in the Chunk
 * Value field. It takes a value from 0 to 254. The value of 255 is
 * reserved for future use as an extension field.
 */

typedef enum {
	ULSTP_CID_DATA              = 0,
	ULSTP_CID_INIT              = 1,
	ULSTP_CID_INIT_ACK          = 2,
	ULSTP_CID_SACK              = 3,
	ULSTP_CID_HEARTBEAT         = 4,
	ULSTP_CID_HEARTBEAT_ACK     = 5,
	ULSTP_CID_ABORT             = 6,
	ULSTP_CID_SHUTDOWN          = 7,
	ULSTP_CID_SHUTDOWN_ACK      = 8,
	ULSTP_CID_ERROR             = 9,
	ULSTP_CID_COOKIE_ECHO       = 10,
	ULSTP_CID_COOKIE_ACK        = 11,
	ULSTP_CID_ECN_ECNE          = 12,
	ULSTP_CID_ECN_CWR           = 13,
	ULSTP_CID_SHUTDOWN_COMPLETE = 14,
	/* AUTH Extension Section 4.1 */
	ULSTP_CID_AUTH              = 0x0F,
	/* PR-ULSTP Sec 3.2 */
	ULSTP_CID_FWD_TSN           = 0xC0,
	/* Use hex, as defined in ADDIP sec. 3.1 */
	ULSTP_CID_ASCONF            = 0xC1,
	ULSTP_CID_ASCONF_ACK        = 0x80,
} ulstp_cid_t; /* enum */

typedef
struct ulstp_datahdr {
	uint32_t tsn;
	uint16_t stream;
	uint16_t ssn;
	uint32_t ppid;
	uint8_t  payload[0];
} __attribute__((packed)) ulstp_datahdr_t;

typedef
struct ulstp_data_chunk {
	ulstp_chunkhdr_t chunk_hdr;
	ulstp_datahdr_t  data_hdr;
} __attribute__((packed)) ulstp_data_chunk_t;

/* DATA Chuck Specific Flags */
enum {
	ULSTP_DATA_MIDDLE_FRAG = 0x00,
	ULSTP_DATA_LAST_FRAG   = 0x01,
	ULSTP_DATA_FIRST_FRAG  = 0x02,
	ULSTP_DATA_NOT_FRAG    = 0x03,
	ULSTP_DATA_UNORDERED   = 0x04,
	ULSTP_DATA_SACK_IMM    = 0x08,
};
enum { ULSTP_DATA_FRAG_MASK = 0x03, };

struct uls_private_data_t * __uls_private_data();

#endif
