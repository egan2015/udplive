#include <stdlib.h>
#include <math.h>
#include "bitops.h"
#include "bitmap.h"


static inline unsigned long __ffs(unsigned long word)
{
  int num = 0;

#if BITS_PER_LONG == 64
  if ((word & 0xffffffff) == 0) {
    num += 32;
    word >>= 32;
  }
#endif
  if ((word & 0xffff) == 0) {
    num += 16;
    word >>= 16;
  }
  if ((word & 0xff) == 0) {
    num += 8;
    word >>= 8;
  }
  if ((word & 0xf) == 0) {
    num += 4;
    word >>= 4;
  }
  if ((word & 0x3) == 0) {
    num += 2;
    word >>= 2;
  }
  if ((word & 0x1) == 0)
    num += 1;
  return num;
}
/**
 * __fls - find last (most-significant) set bit in a long word
 * @word: the word to search
 *
 * Undefined if no set bit exists, so code should check against 0 first.
 */
static inline unsigned long __fls(unsigned long word)
{
  int num = BITS_PER_LONG - 1;

#if BITS_PER_LONG == 64
  if (!(word & (~0ul << 32))) {
    num -= 32;
    word <<= 32;
  }
#endif
  if (!(word & (~0ul << (BITS_PER_LONG - 16)))) {
    num -= 16;
    word <<= 16;
  }
  if (!(word & (~0ul << (BITS_PER_LONG - 8)))) {
    num -= 8;
    word <<= 8;
  }
  if (!(word & (~0ul << (BITS_PER_LONG - 4)))) {
    num -= 4;
    word <<= 4;
  }
  if (!(word & (~0ul << (BITS_PER_LONG - 2)))) {
    num -= 2;
    word <<= 2;
  }
  if (!(word & (~0ul << (BITS_PER_LONG - 1))))
    num -= 1;
  return num;
}


#define ffz(x)  __ffs(~(x))

/*
 * This is a common helper function for find_next_bit and
 * find_next_zero_bit.  The difference is the "invert" argument, which
 * is XORed with each fetched word before searching it for one bits.
 */
static unsigned long _find_next_bit(const unsigned long *addr,
                                    unsigned long nbits, unsigned long start, unsigned long invert)
{
  unsigned long tmp;

  if (!nbits || start >= nbits)
    return nbits;

  tmp = addr[start / BITS_PER_LONG] ^ invert;

  /* Handle 1st word. */
  tmp &= BITMAP_FIRST_WORD_MASK(start);
  start = round_down(start, BITS_PER_LONG);

  while (!tmp) {
    start += BITS_PER_LONG;
    if (start >= nbits)
      return nbits;
    tmp = addr[start / BITS_PER_LONG] ^ invert;
  }

  return min(start + __ffs(tmp), nbits);
}


/*
 * Find the next set bit in a memory region.
 */
unsigned long find_next_bit(const unsigned long *addr, unsigned long size,
                            unsigned long offset)
{
  return _find_next_bit(addr, size, offset, 0UL);
}


unsigned long find_next_zero_bit(const unsigned long *addr, unsigned long size,
                                 unsigned long offset)
{
  return _find_next_bit(addr, size, offset, ~0UL);
}
/*
 * Find the first set bit in a memory region.
 */
unsigned long find_first_bit(const unsigned long *addr, unsigned long size)
{
  unsigned long idx;

  for (idx = 0; idx * BITS_PER_LONG < size; idx++) {
    if (addr[idx])
      return min(idx * BITS_PER_LONG + __ffs(addr[idx]), size);
  }

  return size;
}
/*
 * Find the first cleared bit in a memory region.
 */
unsigned long find_first_zero_bit(const unsigned long *addr, unsigned long size)
{
  unsigned long idx;

  for (idx = 0; idx * BITS_PER_LONG < size; idx++) {
    if (addr[idx] != ~0UL)
      return min(idx * BITS_PER_LONG + ffz(addr[idx]), size);
  }

  return size;
}
unsigned long find_last_bit(const unsigned long *addr, unsigned long size)
{
  if (size) {
    unsigned long val = BITMAP_LAST_WORD_MASK(size);
    unsigned long idx = (size - 1) / BITS_PER_LONG;

    do {
      val &= addr[idx];
      if (val)
        return idx * BITS_PER_LONG + __fls(val);

      val = ~0ul;
    } while (idx--);
  }
  return size;
}
