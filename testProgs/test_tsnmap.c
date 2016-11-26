#include <stdio.h>
#include "vlstp.h"
#include "tsnmap.h"
#include <errno.h>

#define DUMP_CORE {            \
  char *diediedie = 0;         \
  printf("DUMP_CORE %s: %d\n", __FILE__, __LINE__);\
  *diediedie = 0;          \
}


#define TEST_TSNMAP_LEN 64
#define MAGIC_NUMBER 0x42424242

struct {
  struct vlstp_tsnmap global_map;
  unsigned char storage[64 >> 3];
  uint32_t scribble;
} storage;

/* Function prototypes */
void test_tsnmap(uint32_t initial_tsn, uint16_t size);


int
main(int argc, char * const argv[])
{
  /* Test normally. */
  printf("\nTesting full length of map.\n");
  test_tsnmap(0x12345678, TEST_TSNMAP_LEN);

  /* Test map with odd length. */
  printf("\nTesting odd length map.\n");
  test_tsnmap(0x12345678, TEST_TSNMAP_LEN - 1);

  /* Test with initial TSN around TSN rollover */
  printf("\nTesting with initial tsn around rollover 0xffffffff.\n");
  test_tsnmap(0xffffffff, TEST_TSNMAP_LEN);

  /* Test with initial TSN around TSN rollover */
  printf("\nTesting with initial tsn around rollover %x.\n",
         0xffffffff - TEST_TSNMAP_LEN * 2);
  test_tsnmap(0xffffffff - TEST_TSNMAP_LEN * 2, TEST_TSNMAP_LEN);

  printf("%s passes...\n", argv[0]);
  exit(0);

} /* main( ) */


/* This is the main test function.   Its input parameters can be
 * manipulated to run this set of tests with multiple configurations of
 * initial_tsn and map size.
 */
void
test_tsnmap(uint32_t initial_tsn, uint16_t size)
{
  struct vlstp_tsnmap *map;
  uint8_t map_buf[sizeof(struct vlstp_tsnmap) + (VLSTP_TSN_MAP_SIZE >> 3)];
  int i;
  struct vlstp_tsnmap_iter iter;
  uint16_t start, end;

  /* Phase 1
   *
   */

  printf("Testing new tsnmap\n");

  map = vlstp_tsnmap_init((struct vlstp_tsnmap *)&map_buf, size,
                          initial_tsn);
  if (NULL == map) { DUMP_CORE; }

  printf("Testing initial ctsn\n");

  /* Verify cumulative tsn ack point */
  if (!(initial_tsn - 1 == vlstp_tsnmap_get_ctsn(map))) {
    DUMP_CORE;
  }

  /* Make sure that we can mark out-of-range things and not
   * die.
   */
  vlstp_tsnmap_mark(map, initial_tsn - 10);
  vlstp_tsnmap_mark(map, initial_tsn + 2 * size + 10);


  /* Check that mark does not move ctsn if there is a gap. */
  printf("Testing with missing tsns\n");

  vlstp_tsnmap_mark(map, initial_tsn + 1);
  vlstp_tsnmap_mark(map, initial_tsn + size);

  /* Cumulative TSN ack point shouldn't have moved.  */
  /* Verify cumulative tsn ack point.  */
  if (!(initial_tsn - 1 == vlstp_tsnmap_get_ctsn(map))) {
    DUMP_CORE;
  }



  /* Simple checks */

  printf("Testing tsnmap_check\n");

  /*  Has tsn been seen? We haven't marked it, so no.  */
  if (0 != vlstp_tsnmap_check(map, initial_tsn)) {
    DUMP_CORE;
  }

  /* Duplicate Checks */
  /*   Check what happens if tsn arrives lower than what
   *   we are tracking.   We should see this as a dup.
   */
  if (vlstp_tsnmap_check(map, initial_tsn - 1) <= 0) {
    DUMP_CORE;
  }

  /* We marked this tsn so we should see this as a dup.  */
  if (vlstp_tsnmap_check(map, initial_tsn + 1) <= 0) {
    DUMP_CORE;
  }


  /* Lets get the cumulative tsn ack point to move.  */
  printf("Testing ctsn movement\n");

  vlstp_tsnmap_mark(map, initial_tsn);
  if (!(initial_tsn + 1 == vlstp_tsnmap_get_ctsn(map))) {
    DUMP_CORE;
  }

  /* Simple rollover test.  This should force a switch
   * and then switch back and then once again.
   */
  printf("Testing simple rollover\n");

  for (i = 0; i <= size * 3; i++) {
    if (vlstp_tsnmap_check(map, initial_tsn + i) < 0) {
      DUMP_CORE;
    }
    vlstp_tsnmap_mark(map, initial_tsn + i);
  }

  /* The cumulative tsn ack point should have moved with us. */
  if (initial_tsn + size * 3 != vlstp_tsnmap_get_ctsn(map)) {
    DUMP_CORE;
  }


  /* Phase 2
   *
   * Okay now let's allocate a tsnmap from our own memory.
   * As a first test let's just try to go initialize the tsnmap.
   * This should fail as we have not allocated any memory to back
   * the mapping array.
   */

  printf("Testing statically allocated map\n");

  map = &storage.global_map;
  storage.scribble = MAGIC_NUMBER;
  if (NULL == vlstp_tsnmap_init(map, size, initial_tsn)) {
    DUMP_CORE;
  }

  /* This is a more interesting rollover test, lets fill
  * overflow first, followed by filling the first mapping.
  */
  printf("Testing rollover to a full overflow\n");

  /* Fill the overflow.  */
  for (i = size; i < size * 2; i++) {
    if (vlstp_tsnmap_check(map, initial_tsn + i) < 0) {
      printf("check=%d\n", vlstp_tsnmap_check(map, initial_tsn + i));
      DUMP_CORE;
    }
    vlstp_tsnmap_mark(map, initial_tsn + i);
  }

  if (MAGIC_NUMBER != storage.scribble) {
    DUMP_CORE;
  }

  /* Verify the cumulative tsn ack point. */
  if (initial_tsn - 1 != vlstp_tsnmap_get_ctsn(map)) {
    DUMP_CORE;
  }

  /* Fill the first map, except for initial_tsn. */
  for (i = 1; i < size; i++) {
    if (vlstp_tsnmap_check(map, initial_tsn + i) < 0) {
      DUMP_CORE;
    }
    vlstp_tsnmap_mark(map, initial_tsn + i);
  }

  if (MAGIC_NUMBER != storage.scribble) {
    DUMP_CORE;
  }

  /* Verify that the cumulative tsn ack point has not moved. */
  if (!(initial_tsn - 1 == vlstp_tsnmap_get_ctsn(map))) {
    DUMP_CORE;
  }


  /* Mark the initial TSN, both maps should be full and the
   * Cumulative TSN ACK Point should move ahead finally.
   */
  vlstp_tsnmap_mark(map, initial_tsn);

  /* Verify that the cumulative tsn ack point has moved */
  if (!(initial_tsn + size * 2 - 1 == vlstp_tsnmap_get_ctsn(map))) {
    printf("initial_tsn %ld , ctsn %ld\n", (initial_tsn + size * 2 - 1),
           vlstp_tsnmap_get_ctsn(map));
    DUMP_CORE;
  }


  /* Big Gap Test
   *
   */

  printf("Testing big gap\n");

  /* At this point, the first map should be full and the overflow
   * is empty.  Let's check beyond what we can handle, this check
   * is very specific to the implementation but this seems fair
   * given this is a unittest.
   */
  goto next;
  if (0 <= vlstp_tsnmap_check(map, initial_tsn + size * 4)) {
    DUMP_CORE;
  }

next:
  /* Now check one under the previously checked tsn.  This
   * new tsn should show up as new
   */
  if (0 != vlstp_tsnmap_check(map, initial_tsn + size * 4 - 1)) {
    DUMP_CORE;
  }


  /* Phase 3
   *
   */

  /* Reinit the tsnmap back to sanity.  */
  if (NULL == vlstp_tsnmap_init(map,
                                size, initial_tsn)) {
    DUMP_CORE;
  }


  /* Let's test getting the gap ack blocks.  */

  printf("Testing gap ack blocks\n");
  vlstp_tsnmap_iter_init(map, &iter);

  if (0 != vlstp_tsnmap_next_gap_ack(map, &iter, &start, &end)) {
    DUMP_CORE;
  }

  printf("start %ld,end %ld\n", start , end);
  /* Create a small gap and test.  */

  printf("Testing for single ack block\n");
  vlstp_tsnmap_mark(map, initial_tsn + 2);


  /* Count the number of Gap Ack Blocks.  */
  vlstp_tsnmap_iter_init(map, &iter);
  for (i = 0;
       vlstp_tsnmap_next_gap_ack(map, &iter, &start, &end);
       i++) {
    /* There should only be one ack. */
    if (i > 1) {
      DUMP_CORE;
    }
  }

  /* Check the start and end of the gap.  */
  if (   3 != start
         || 3 != end ) {
    DUMP_CORE;
  }

  /* Lets create two more ack blocks.  */
  printf("Testing with 2 more blocks\n");

  /* Add to the existing block. */
  vlstp_tsnmap_mark(map, initial_tsn + 3);
  vlstp_tsnmap_mark(map, initial_tsn + 4);


  /* Create another block. */
  vlstp_tsnmap_mark(map, initial_tsn + size - 3);
  vlstp_tsnmap_mark(map, initial_tsn + size - 2);

  /* Create a gap across maps.  */
  vlstp_tsnmap_mark(map, initial_tsn + size + 3);


  /* Let's hand check each of the blocks. */
  vlstp_tsnmap_iter_init(map, &iter);

  /* Block #1 */
  printf("Testing Block #1\n");
  if (!vlstp_tsnmap_next_gap_ack(map, &iter, &start, &end)) {
    DUMP_CORE;
  }
  if ((3 != start) || (5 != end)) {
    DUMP_CORE;
  }

  /* Block #2 */

  printf("Testing Block #2\n");
  if (!vlstp_tsnmap_next_gap_ack(map, &iter, &start, &end)) {
    DUMP_CORE;
  }
  if (((size - 3 + 1) != start) || ((size - 2 + 1) != end)) {
    DUMP_CORE;
  }


  /* Block #3 */
  printf("Testing Block #3 (in the overflow map)\n");
  if (!vlstp_tsnmap_next_gap_ack(map, &iter, &start, &end)) {
    DUMP_CORE;
  }

  if (((size + 3 + 1) != start) || ((size + 3 + 1) != end)) {
    DUMP_CORE;
  }

  /* There should be no more gaps.  */
  if (vlstp_tsnmap_next_gap_ack(map, &iter, &start, &end)) {
    DUMP_CORE;
  }

  printf("Testing block after ctsn moves beyond previous ack block.\n");

  /* Test block when ctsn is in the map.  */
  /* Let's blow away the first block by moving ctsn into it.  */
  vlstp_tsnmap_mark(map, initial_tsn);
  vlstp_tsnmap_mark(map, initial_tsn + 1);

  if (initial_tsn + 4 != vlstp_tsnmap_get_ctsn(map)) {
    DUMP_CORE;
  }

  vlstp_tsnmap_iter_init(map, &iter);
  if (0 == vlstp_tsnmap_next_gap_ack(map, &iter, &start, &end)) {
    DUMP_CORE;
  }

  /* The ctsn jumped by 5; the block previously started at size - 2 + 1
   * and ended at size - 3 + 1.
   */
  if ( size - 3 - 4 != start
       || size - 2 - 4 != end) {
    DUMP_CORE;
  }


  /* Check that we don't lose ack information when we switch maps.
   * Check this by filling up the first map.
   * The first two tsns of the overflow_map are empty--we will
   * force a switch by filling the first map and the first entry
   * of the overflow map.
   */
  printf("Test rollover not losing overflow's ack information\n");
  for (i = 0; i <= size; i++) {
    vlstp_tsnmap_mark(map, initial_tsn + i);
  }

  if (MAGIC_NUMBER != storage.scribble) {
    DUMP_CORE;
  }

  /* Sanity check the Cumulative TSN Ack Point */
  if (initial_tsn + size != vlstp_tsnmap_get_ctsn(map)) {
    DUMP_CORE;
  }

  /* Check that the ack block that we know exists gets returned. */
  vlstp_tsnmap_iter_init(map, &iter);
  if (0 == vlstp_tsnmap_next_gap_ack(map, &iter, &start, &end)) {
    DUMP_CORE;
  }

  /* Now hand check that the ack block looks correct.  */
  if ((3 != start) || (3 != end)) {
    DUMP_CORE;
  }

  if (MAGIC_NUMBER != storage.scribble) {
    DUMP_CORE;
  }


  /* Phase 4
  *
  */

  /* Reinit the tsnmap back to sanity.  */
  if (NULL == vlstp_tsnmap_init(map,
                                size, initial_tsn)) {
    DUMP_CORE;
  }


  /* Test with a gap ack block precisely at the end of the overflow. */
  printf("Testing block ending precisely at end of overflow map.\n");
  vlstp_tsnmap_mark(map, initial_tsn + 2 * size - 1);

  /* Check that we find the gap ack block we expect. */
  vlstp_tsnmap_iter_init(map, &iter);
  if (0 == vlstp_tsnmap_next_gap_ack(map, &iter, &start, &end)) {
    DUMP_CORE;
  }

  /* Now hand check that the gap ack block looks correct.  */
  if ((size * 2 != start)
      || (size * 2 != end)) {
    DUMP_CORE;
  }

  /* Test with a gap ack block precisely at the end of the tsnmap. */
  printf("Testing block ending precisely at end of first map.\n");
  vlstp_tsnmap_mark(map, initial_tsn + size - 1);

  /* Check for the first ack block. */
  vlstp_tsnmap_iter_init(map, &iter);
  if (0 == vlstp_tsnmap_next_gap_ack(map, &iter, &start, &end)) {
    DUMP_CORE;
  }


  /* Now hand check that the ack block looks correct.  */
  if (size != start
      || size != end) {
    DUMP_CORE;
  }

  /* Check the second block */
  if (0 == vlstp_tsnmap_next_gap_ack(map, &iter, &start, &end)) {
    DUMP_CORE;
  }

  /* Now hand check that the ack block looks correct.  */
  if (size * 2 != start
      || size * 2 != end) {
    DUMP_CORE;
  }


  /* Now create a gap ack block across the maps.   We've previously
  * tested having a gap across the maps.
  */
  printf("Testing block across map boundaries.\n");
  vlstp_tsnmap_mark(map, initial_tsn + size);

  /* Check that the first ack block. */
  vlstp_tsnmap_iter_init(map, &iter);
  if (0 == vlstp_tsnmap_next_gap_ack(map, &iter, &start, &end)) {
    DUMP_CORE;
  }


  /* Now hand check that the ack block looks correct.  */
  if (size != start ||
      size + 1 != end) {
    DUMP_CORE;
  }

  /* Check the second block */
  if (0 == vlstp_tsnmap_next_gap_ack(map, &iter, &start, &end)) {
    DUMP_CORE;
  }


  /* Now hand check that the ack block looks correct.  */
  if (size * 2 != start ||
      size * 2 != end) {
    DUMP_CORE;
  }


  if (MAGIC_NUMBER != storage.scribble) {
    DUMP_CORE;
  }

} /* test_tsnmap() */