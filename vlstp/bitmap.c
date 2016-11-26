#include "bitmap.h"
/*
 * bitmaps provide an array of bits, implemented using an an
 * array of unsigned longs.  The number of valid bits in a
 * given bitmap does _not_ need to be an exact multiple of
 * BITS_PER_LONG.
 *
 * The possible unused bits in the last, partially used word
 * of a bitmap are 'don't care'.  The implementation makes
 * no particular effort to keep them zero.  It ensures that
 * their value will not affect the results of any operation.
 * The bitmap operations that return Boolean (bitmap_empty,
 * for example) or scalar (bitmap_weight, for example) results
 * carefully filter out these unused bits from impacting their
 * results.
 *
 * These operations actually hold to a slightly stronger rule:
 * if you don't input any bitmaps to these ops that have some
 * unused bits set, then they won't output any set unused bits
 * in output bitmaps.
 *
 * The byte ordering of bitmaps is more natural on little
 * endian architectures.  See the big-endian headers
 * include/asm-ppc64/bitops.h and include/asm-s390/bitops.h
 * for the best explanations of this ordering.
 */

int __bitmap_equal(const unsigned long *bitmap1,
                   const unsigned long *bitmap2, unsigned int bits)
{
  unsigned int k, lim = bits / BITS_PER_LONG;
  for (k = 0; k < lim; ++k)
    if (bitmap1[k] != bitmap2[k])
      return 0;

  if (bits % BITS_PER_LONG)
    if ((bitmap1[k] ^ bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits))
      return 0;

  return 1;
}

void __bitmap_complement(unsigned long *dst, const unsigned long *src, unsigned int bits)
{
  unsigned int k, lim = bits / BITS_PER_LONG;
  for (k = 0; k < lim; ++k)
    dst[k] = ~src[k];

  if (bits % BITS_PER_LONG)
    dst[k] = ~src[k];
}


/**
 * __bitmap_shift_right - logical right shift of the bits in a bitmap
 *   @dst : destination bitmap
 *   @src : source bitmap
 *   @shift : shift by this many bits
 *   @nbits : bitmap size, in bits
 *
 * Shifting right (dividing) means moving bits in the MS -> LS bit
 * direction.  Zeros are fed into the vacated MS positions and the
 * LS bits shifted off the bottom are lost.
 */
void __bitmap_shift_right(unsigned long *dst, const unsigned long *src,
                          unsigned shift, unsigned nbits)
{
  unsigned k, lim = BITS_TO_LONGS(nbits);
  unsigned off = shift / BITS_PER_LONG, rem = shift % BITS_PER_LONG;
  unsigned long mask = BITMAP_LAST_WORD_MASK(nbits);
  for (k = 0; off + k < lim; ++k) {
    unsigned long upper, lower;

    /*
     * If shift is not word aligned, take lower rem bits of
     * word above and make them the top rem bits of result.
     */
    if (!rem || off + k + 1 >= lim)
      upper = 0;
    else {
      upper = src[off + k + 1];
      if (off + k + 1 == lim - 1)
        upper &= mask;
      upper <<= (BITS_PER_LONG - rem);
    }
    lower = src[off + k];
    if (off + k == lim - 1)
      lower &= mask;
    lower >>= rem;
    dst[k] = lower | upper;
  }
  if (off)
    memset(&dst[lim - off], 0, off * sizeof(unsigned long));
}


/**
 * __bitmap_shift_left - logical left shift of the bits in a bitmap
 *   @dst : destination bitmap
 *   @src : source bitmap
 *   @shift : shift by this many bits
 *   @nbits : bitmap size, in bits
 *
 * Shifting left (multiplying) means moving bits in the LS -> MS
 * direction.  Zeros are fed into the vacated LS bit positions
 * and those MS bits shifted off the top are lost.
 */

void __bitmap_shift_left(unsigned long *dst, const unsigned long *src,
                         unsigned int shift, unsigned int nbits)
{
  int k;
  unsigned int lim = BITS_TO_LONGS(nbits);
  unsigned int off = shift / BITS_PER_LONG, rem = shift % BITS_PER_LONG;
  for (k = lim - off - 1; k >= 0; --k) {
    unsigned long upper, lower;

    /*
     * If shift is not word aligned, take upper rem bits of
     * word below and make them the bottom rem bits of result.
     */
    if (rem && k > 0)
      lower = src[k - 1] >> (BITS_PER_LONG - rem);
    else
      lower = 0;
    upper = src[k] << rem;
    dst[k + off] = lower | upper;
  }
  if (off)
    memset(dst, 0, off * sizeof(unsigned long));
}

int __bitmap_and(unsigned long *dst, const unsigned long *bitmap1,
                 const unsigned long *bitmap2, unsigned int bits)
{
  unsigned int k;
  unsigned int lim = bits / BITS_PER_LONG;
  unsigned long result = 0;

  for (k = 0; k < lim; k++)
    result |= (dst[k] = bitmap1[k] & bitmap2[k]);
  if (bits % BITS_PER_LONG)
    result |= (dst[k] = bitmap1[k] & bitmap2[k] &
                        BITMAP_LAST_WORD_MASK(bits));
  return result != 0;
}

void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1,
                 const unsigned long *bitmap2, unsigned int bits)
{
  unsigned int k;
  unsigned int nr = BITS_TO_LONGS(bits);

  for (k = 0; k < nr; k++)
    dst[k] = bitmap1[k] | bitmap2[k];
}


void __bitmap_xor(unsigned long *dst, const unsigned long *bitmap1,
                  const unsigned long *bitmap2, unsigned int bits)
{
  unsigned int k;
  unsigned int nr = BITS_TO_LONGS(bits);

  for (k = 0; k < nr; k++)
    dst[k] = bitmap1[k] ^ bitmap2[k];
}


int __bitmap_andnot(unsigned long *dst, const unsigned long *bitmap1,
                    const unsigned long *bitmap2, unsigned int bits)
{
  unsigned int k;
  unsigned int lim = bits / BITS_PER_LONG;
  unsigned long result = 0;

  for (k = 0; k < lim; k++)
    result |= (dst[k] = bitmap1[k] & ~bitmap2[k]);
  if (bits % BITS_PER_LONG)
    result |= (dst[k] = bitmap1[k] & ~bitmap2[k] &
                        BITMAP_LAST_WORD_MASK(bits));
  return result != 0;
}


int __bitmap_intersects(const unsigned long *bitmap1,
                        const unsigned long *bitmap2, unsigned int bits)
{
  unsigned int k, lim = bits / BITS_PER_LONG;
  for (k = 0; k < lim; ++k)
    if (bitmap1[k] & bitmap2[k])
      return 1;

  if (bits % BITS_PER_LONG)
    if ((bitmap1[k] & bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits))
      return 1;
  return 0;
}

int __bitmap_subset(const unsigned long *bitmap1,
                    const unsigned long *bitmap2, unsigned int bits)
{
  unsigned int k, lim = bits / BITS_PER_LONG;
  for (k = 0; k < lim; ++k)
    if (bitmap1[k] & ~bitmap2[k])
      return 0;

  if (bits % BITS_PER_LONG)
    if ((bitmap1[k] & ~bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits))
      return 0;
  return 1;
}

int __bitmap_weight(const unsigned long *bitmap, unsigned int bits)
{
  unsigned int k, lim = bits / BITS_PER_LONG;
  int w = 0;

  for (k = 0; k < lim; k++)
    w += hweight_long(bitmap[k]);

  if (bits % BITS_PER_LONG)
    w += hweight_long(bitmap[k] & BITMAP_LAST_WORD_MASK(bits));

  return w;
}

void bitmap_set(unsigned long *map, unsigned int start, int len)
{
  unsigned long *p = map + BIT_WORD(start);
  const unsigned int size = start + len;
  int bits_to_set = BITS_PER_LONG - (start % BITS_PER_LONG);
  unsigned long mask_to_set = BITMAP_FIRST_WORD_MASK(start);

  while (len - bits_to_set >= 0) {
    *p |= mask_to_set;
    len -= bits_to_set;
    bits_to_set = BITS_PER_LONG;
    mask_to_set = ~0UL;
    p++;
  }
  if (len) {
    mask_to_set &= BITMAP_LAST_WORD_MASK(size);
    *p |= mask_to_set;
  }
}

void bitmap_clear(unsigned long *map, unsigned int start, int len)
{
  unsigned long *p = map + BIT_WORD(start);
  const unsigned int size = start + len;
  int bits_to_clear = BITS_PER_LONG - (start % BITS_PER_LONG);
  unsigned long mask_to_clear = BITMAP_FIRST_WORD_MASK(start);

  while (len - bits_to_clear >= 0) {
    *p &= ~mask_to_clear;
    len -= bits_to_clear;
    bits_to_clear = BITS_PER_LONG;
    mask_to_clear = ~0UL;
    p++;
  }
  if (len) {
    mask_to_clear &= BITMAP_LAST_WORD_MASK(size);
    *p &= ~mask_to_clear;
  }
}

/**
 * bitmap_find_next_zero_area_off - find a contiguous aligned zero area
 * @map: The address to base the search on
 * @size: The bitmap size in bits
 * @start: The bitnumber to start searching at
 * @nr: The number of zeroed bits we're looking for
 * @align_mask: Alignment mask for zero area
 * @align_offset: Alignment offset for zero area.
 *
 * The @align_mask should be one less than a power of 2; the effect is that
 * the bit offset of all zero areas this function finds plus @align_offset
 * is multiple of that power of 2.
 */
unsigned long bitmap_find_next_zero_area_off(unsigned long *map,
    unsigned long size,
    unsigned long start,
    unsigned int nr,
    unsigned long align_mask,
    unsigned long align_offset)
{
  unsigned long index, end, i;
again:
  index = find_next_zero_bit(map, size, start);

  /* Align allocation */
  index = __ALIGN_MASK(index + align_offset, align_mask) - align_offset;

  end = index + nr;
  if (end > size)
    return end;
  i = find_next_bit(map, end, index);
  if (i < end) {
    start = i + 1;
    goto again;
  }
  return index;
}


/**
 * bitmap_pos_to_ord - find ordinal of set bit at given position in bitmap
 *  @buf: pointer to a bitmap
 *  @pos: a bit position in @buf (0 <= @pos < @nbits)
 *  @nbits: number of valid bit positions in @buf
 *
 * Map the bit at position @pos in @buf (of length @nbits) to the
 * ordinal of which set bit it is.  If it is not set or if @pos
 * is not a valid bit position, map to -1.
 *
 * If for example, just bits 4 through 7 are set in @buf, then @pos
 * values 4 through 7 will get mapped to 0 through 3, respectively,
 * and other @pos values will get mapped to -1.  When @pos value 7
 * gets mapped to (returns) @ord value 3 in this example, that means
 * that bit 7 is the 3rd (starting with 0th) set bit in @buf.
 *
 * The bit positions 0 through @bits are valid positions in @buf.
 */
static int bitmap_pos_to_ord(const unsigned long *buf, unsigned int pos,
                             unsigned int nbits)
{
  if (pos >= nbits || !test_bit(pos, buf))
    return -1;

  return __bitmap_weight(buf, pos);
}

/**
 * bitmap_ord_to_pos - find position of n-th set bit in bitmap
 *  @buf: pointer to bitmap
 *  @ord: ordinal bit position (n-th set bit, n >= 0)
 *  @nbits: number of valid bit positions in @buf
 *
 * Map the ordinal offset of bit @ord in @buf to its position in @buf.
 * Value of @ord should be in range 0 <= @ord < weight(buf). If @ord
 * >= weight(buf), returns @nbits.
 *
 * If for example, just bits 4 through 7 are set in @buf, then @ord
 * values 0 through 3 will get mapped to 4 through 7, respectively,
 * and all other @ord values returns @nbits.  When @ord value 3
 * gets mapped to (returns) @pos value 7 in this example, that means
 * that the 3rd set bit (starting with 0th) is at position 7 in @buf.
 *
 * The bit positions 0 through @nbits-1 are valid positions in @buf.
 */
unsigned int bitmap_ord_to_pos(const unsigned long *buf, unsigned int ord,
                               unsigned int nbits)
{
  unsigned int pos;

  for (pos = find_first_bit(buf, nbits);
       pos < nbits && ord;
       pos = find_next_bit(buf, nbits, pos + 1))
    ord--;

  return pos;
}

/**
 * bitmap_fold - fold larger bitmap into smaller, modulo specified size
 *  @dst: resulting smaller bitmap
 *  @orig: original larger bitmap
 *  @sz: specified size
 *  @nbits: number of bits in each of these bitmaps
 *
 * For each bit oldbit in @orig, set bit oldbit mod @sz in @dst.
 * Clear all other bits in @dst.  See further the comment and
 * Example [2] for bitmap_onto() for why and how to use this.
 */
void bitmap_fold(unsigned long *dst, const unsigned long *orig,
                 unsigned int sz, unsigned int nbits)
{
  unsigned int oldbit;

  if (dst == orig)  /* following doesn't handle inplace mappings */
    return;
  bitmap_zero(dst, nbits);

  for_each_set_bit(oldbit, orig, nbits)
  set_bit(oldbit % sz, dst);
}



