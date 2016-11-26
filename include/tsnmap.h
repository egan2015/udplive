#ifndef __tsnmap_h__
#define __tsnmap_h__
/* RFC 2960 12.2 Parameters necessary per association (i.e. the TCB)
 * Mapping  An array of bits or bytes indicating which out of
 * Array    order TSN's have been received (relative to the
 *          Last Rcvd TSN). If no gaps exist, i.e. no out of
 *          order packets have been received, this array
 *          will be set to all zero. This structure may be
 *          in the form of a circular buffer or bit array.
 */

/* Guess at how big to make the TSN mapping array.
 * We guarantee that we can handle at least this big a gap between the
 * cumulative ACK and the highest TSN.  In practice, we can often
 * handle up to twice this value.
 *
 * NEVER make this more than 32767 (2^15-1).  The Gap Ack Blocks in a
 * SACK (see  section 3.3.4) are only 16 bits, so 2*SCTP_TSN_MAP_SIZE
 * must be less than 65535 (2^16 - 1), or we will have overflow
 * problems creating SACK's.
 */
#define VLSTP_TSN_MAP_INITIAL BITS_PER_LONG
#define VLSTP_TSN_MAP_INCREMENT VLSTP_TSN_MAP_INITIAL
#define VLSTP_TSN_MAP_SIZE 4096

/* We will not record more than this many duplicate TSNs between two
 * SACKs.  The minimum PMTU is 576.  Remove all the headers and there
 * is enough room for 131 duplicate reports.  Round down to the
 * nearest power of 2.
 */
enum { SCTP_MIN_PMTU = 576 };
enum { VLSTP_MAX_DUP_TSNS = 16 };
enum { VLSTP_MAX_GABS = 16 };

/*
 * 3.3.4 Selective Acknowledgement (SACK) (3):
 *
 *  This chunk is sent to the peer endpoint to acknowledge received DATA
 *  chunks and to inform the peer endpoint of gaps in the received
 *  subsequences of DATA chunks as represented by their TSNs.
 */

typedef struct vlstp_gap_ack_block {
  uint16_t start;
  uint16_t end;
} __attribute__((packed)) sctp_gap_ack_block_t;


struct vlstp_tsnmap {
  /* This array counts the number of chunks with each TSN.
   * It points at one of the two buffers with which we will
   * ping-pong between.
   */
  unsigned long *tsn_map;

  /* This is the TSN at tsn_map[0].  */
  uint32_t base_tsn;

  /* Last Rcvd   : This is the last TSN received in
   * TSN         : sequence. This value is set initially by
   *             : taking the peer's Initial TSN, received in
   *             : the INIT or INIT ACK chunk, and subtracting
   *             : one from it.
   *
   * Throughout most of the specification this is called the
   * "Cumulative TSN ACK Point".  In this case, we
   * ignore the advice in 12.2 in favour of the term
   * used in the bulk of the text.
   */
  uint32_t cumulative_tsn_ack_point;

  /* This is the highest TSN we've marked.  */
  uint32_t max_tsn_seen;

  /* This is the minimum number of TSNs we can track.  This corresponds
   * to the size of tsn_map.   Note: the overflow_map allows us to
   * potentially track more than this quantity.
   */
  uint16_t len;

  /* Data chunks pending receipt. used by vlstp_STATUS sockopt */
  uint16_t pending_data;

  /* Record duplicate TSNs here.  We clear this after
   * every SACK.  Store up to vlstp_MAX_DUP_TSNS worth of
   * information.
   */
  uint16_t num_dup_tsns;
  uint32_t dup_tsns[VLSTP_MAX_DUP_TSNS];
};

struct vlstp_tsnmap_iter {
  uint32_t start;
};

/* Initialize a Gap Ack Block iterator from memory being provided.  */
void vlstp_tsnmap_iter_init(const struct vlstp_tsnmap *map,
                            struct vlstp_tsnmap_iter *iter);

/* Initialize a block of memory as a tsnmap.  */
struct vlstp_tsnmap *vlstp_tsnmap_init(struct vlstp_tsnmap *, uint16_t len,
                                       uint32_t initial_tsn);

void vlstp_tsnmap_free(struct vlstp_tsnmap *map);

/* Get the next Gap Ack Blocks. Returns 0 if there was not another block
 * to get.
 */
int vlstp_tsnmap_next_gap_ack(const struct vlstp_tsnmap *map,
                              struct vlstp_tsnmap_iter *iter,
                              uint16_t *start, uint16_t *end);

/* Test the tracking state of this TSN.
 * Returns:
 *   0 if the TSN has not yet been seen
 *  >0 if the TSN has been seen (duplicate)
 *  <0 if the TSN is invalid (too large to track)
 */
int vlstp_tsnmap_check(const struct vlstp_tsnmap *, uint32_t tsn);

/* Mark this TSN as seen.  */
int vlstp_tsnmap_mark(struct vlstp_tsnmap *, uint32_t tsn);

/* Mark this TSN and all lower as seen. */
void vlstp_tsnmap_skip(struct vlstp_tsnmap *map, uint32_t tsn);

/* Retrieve the Cumulative TSN ACK Point.  */
static inline uint32_t vlstp_tsnmap_get_ctsn(const struct vlstp_tsnmap *map)
{
  return map->cumulative_tsn_ack_point;
}

/* Retrieve the highest TSN we've seen.  */
static inline uint32_t vlstp_tsnmap_get_max_tsn_seen(const struct vlstp_tsnmap *map)
{
  return map->max_tsn_seen;
}

/* How many duplicate TSNs are stored? */
static inline uint16_t vlstp_tsnmap_num_dups(struct vlstp_tsnmap *map)
{
  return map->num_dup_tsns;
}

/* Return pointer to duplicate tsn array as needed by SACK. */
static inline uint32_t *vlstp_tsnmap_get_dups(struct vlstp_tsnmap *map)
{
  map->num_dup_tsns = 0;
  return map->dup_tsns;
}

/* How many gap ack blocks do we have recorded? */
uint16_t vlstp_tsnmap_num_gabs(struct vlstp_tsnmap *map,
                               struct vlstp_gap_ack_block *gabs);

/* Refresh the count on pending data. */
uint16_t vlstp_tsnmap_pending(struct vlstp_tsnmap *map);

/* Is there a gap in the TSN map?  */
static inline int vlstp_tsnmap_has_gap(const struct vlstp_tsnmap *map)
{
  return map->cumulative_tsn_ack_point != map->max_tsn_seen;
}

/* Mark a duplicate TSN.  Note:  limit the storage of duplicate TSN
 * information.
 */
static inline void vlstp_tsnmap_mark_dup(struct vlstp_tsnmap *map, uint32_t tsn)
{
  if (map->num_dup_tsns < VLSTP_MAX_DUP_TSNS)
    map->dup_tsns[map->num_dup_tsns++] = htonl(tsn);
}

/* Renege a TSN that was seen.  */
void vlstp_tsnmap_renege(struct vlstp_tsnmap *, uint32_t tsn);

/* Is there a gap in the TSN map? */
int vlstp_tsnmap_has_gap(const struct vlstp_tsnmap *);
#endif