#ifndef _vlstp_structs_h
#define _vlstp_structs_h

#include "list.h"
#include <stdint.h>

typedef struct vlstp_hdr {
	uint16_t	source;		/*source port*/
	uint16_t	dest;		/*dest port*/
	uint32_t	vtag;
	uint32_t	checksum;
} __attribute__((packed))	vlstp_hdr_t;

typedef struct vlstp_chunkhdr {
	uint8_t		type;
	uint8_t		flags;
	uint16_t	length;
} __attribute__((packed))	vlstp_chunkhdr_t;

/* Section 3.2.  Chunk Type Values.
 * [Chunk Type] identifies the type of information contained in the Chunk
 * Value field. It takes a value from 0 to 254. The value of 255 is
 * reserved for future use as an extension field.
 */

typedef enum {
	VLSTP_CID_DATA              = 0,
	VLSTP_CID_INIT              = 1,
	VLSTP_CID_INIT_ACK          = 2,
	VLSTP_CID_SACK              = 3,
	VLSTP_CID_HEARTBEAT         = 4,
	VLSTP_CID_HEARTBEAT_ACK     = 5,
	VLSTP_CID_ABORT             = 6,
	VLSTP_CID_SHUTDOWN          = 7,
	VLSTP_CID_SHUTDOWN_ACK      = 8,
	VLSTP_CID_ERROR             = 9,
	VLSTP_CID_COOKIE_ECHO       = 10,
	VLSTP_CID_COOKIE_ACK        = 11,
	VLSTP_CID_ECN_ECNE          = 12,
	VLSTP_CID_ECN_CWR           = 13,
	VLSTP_CID_SHUTDOWN_COMPLETE = 14,
	/* AUTH Extension Section 4.1 */
	VLSTP_CID_AUTH              = 0x0F,
	/* PR-VLSTP Sec 3.2 */
	VLSTP_CID_FWD_TSN           = 0xC0,
	/* Use hex, as defined in ADDIP sec. 3.1 */
	VLSTP_CID_ASCONF            = 0xC1,
	VLSTP_CID_ASCONF_ACK        = 0x80,
} vlstp_cid_t; /* enum */

typedef
struct vlstp_datahdr {
	uint32_t tsn;
	uint16_t stream;
	uint16_t ssn;
	uint32_t ppid;
	uint8_t  payload[0];
} __attribute__((packed)) vlstp_datahdr_t;

typedef
struct vlstp_data_chunk {
	vlstp_chunkhdr_t chunk_hdr;
	vlstp_datahdr_t  data_hdr;
} __attribute__((packed)) vlstp_data_chunk_t;

/* DATA Chuck Specific Flags */
enum {
	VLSTP_DATA_MIDDLE_FRAG = 0x00,
	VLSTP_DATA_LAST_FRAG   = 0x01,
	VLSTP_DATA_FIRST_FRAG  = 0x02,
	VLSTP_DATA_NOT_FRAG    = 0x03,
	VLSTP_DATA_UNORDERED   = 0x04,
	VLSTP_DATA_SACK_IMM    = 0x08,
};
enum { VLSTP_DATA_FRAG_MASK = 0x03, };

#endif
