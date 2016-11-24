#ifndef _rstp_structs_h
#define _rstp_structs_h

#include "list.h"
#include <stdint.h>

struct uls_private_data_t {
	struct list_head timer_list;
	char name[255];
};

typedef struct rstp_hdr {
	uint16_t	source;		/*source port*/
	uint16_t	dest;		/*dest port*/
	uint32_t	vtag;
	uint32_t	checksum;
} __attribute__((packed))	rstp_hdr_t;

typedef struct rstp_chunkhdr {
	uint8_t		type;
	uint8_t		flags;
	uint16_t	length;
} __attribute__((packed))	rstp_chunkhdr_t;

/* Section 3.2.  Chunk Type Values.
 * [Chunk Type] identifies the type of information contained in the Chunk
 * Value field. It takes a value from 0 to 254. The value of 255 is
 * reserved for future use as an extension field.
 */

typedef enum {
	RSTP_CID_DATA              = 0,
	RSTP_CID_INIT              = 1,
	RSTP_CID_INIT_ACK          = 2,
	RSTP_CID_SACK              = 3,
	RSTP_CID_HEARTBEAT         = 4,
	RSTP_CID_HEARTBEAT_ACK     = 5,
	RSTP_CID_ABORT             = 6,
	RSTP_CID_SHUTDOWN          = 7,
	RSTP_CID_SHUTDOWN_ACK      = 8,
	RSTP_CID_ERROR             = 9,
	RSTP_CID_COOKIE_ECHO       = 10,
	RSTP_CID_COOKIE_ACK        = 11,
	RSTP_CID_ECN_ECNE          = 12,
	RSTP_CID_ECN_CWR           = 13,
	RSTP_CID_SHUTDOWN_COMPLETE = 14,
	/* AUTH Extension Section 4.1 */
	RSTP_CID_AUTH              = 0x0F,
	/* PR-RSTP Sec 3.2 */
	RSTP_CID_FWD_TSN           = 0xC0,
	/* Use hex, as defined in ADDIP sec. 3.1 */
	RSTP_CID_ASCONF            = 0xC1,
	RSTP_CID_ASCONF_ACK        = 0x80,
} rstp_cid_t; /* enum */

typedef
struct rstp_datahdr {
	uint32_t tsn;
	uint16_t stream;
	uint16_t ssn;
	uint32_t ppid;
	uint8_t  payload[0];
} __attribute__((packed)) rstp_datahdr_t;

typedef
struct rstp_data_chunk {
	rstp_chunkhdr_t chunk_hdr;
	rstp_datahdr_t  data_hdr;
} __attribute__((packed)) rstp_data_chunk_t;

/* DATA Chuck Specific Flags */
enum {
	RSTP_DATA_MIDDLE_FRAG = 0x00,
	RSTP_DATA_LAST_FRAG   = 0x01,
	RSTP_DATA_FIRST_FRAG  = 0x02,
	RSTP_DATA_NOT_FRAG    = 0x03,
	RSTP_DATA_UNORDERED   = 0x04,
	RSTP_DATA_SACK_IMM    = 0x08,
};
enum { RSTP_DATA_FRAG_MASK = 0x03, };

struct uls_private_data_t * __uls_private_data();

#endif
