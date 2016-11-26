#ifndef __bitops_h__
#define __bitops_h__

#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))
#define ALIGN(x,a)      __ALIGN_MASK(x,(typeof(x))(a)-1)

#define BITS_PER_BYTE   8
#define BITS_PER_LONG   32

#define BIT(nr)     (1UL << (nr))
#define BIT_ULL(nr)   (1ULL << (nr))
#define BIT_MASK(nr)    (1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)    ((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)  (1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)  ((nr) / BITS_PER_LONG_LONG)
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))


#define max(a,b)        (((a) > (b)) ? (a) : (b))
#define min(a,b)        (((a) < (b)) ? (a) : (b))

/*
 * This looks more complex than it should be. But we need to
 * get the type for the ~ right in round_down (it needs to be
 * as wide as the result!), and we want to evaluate the macro
 * arguments just once each.
 */
#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y) ((x) & ~__round_mask(x, y))


#define DECLARE_BITMAP(name,bits) \
  unsigned long name[BITS_TO_LONGS(bits)]

/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK_ULL(39, 21) gives us the 64bit vector 0x000000ffffe00000.
 */
#define GENMASK(h, l) \
  (((~0UL) << (l)) & (~0UL >> (BITS_PER_LONG - 1 - (h))))

#define GENMASK_ULL(h, l) \
  (((~0ULL) << (l)) & (~0ULL >> (BITS_PER_LONG_LONG - 1 - (h))))

/*
 * Compile time versions of __arch_hweightN()
 */
#define __const_hweight8(w)   \
  ((unsigned int)     \
   ((!!((w) & (1ULL << 0))) + \
    (!!((w) & (1ULL << 1))) + \
    (!!((w) & (1ULL << 2))) + \
    (!!((w) & (1ULL << 3))) + \
    (!!((w) & (1ULL << 4))) + \
    (!!((w) & (1ULL << 5))) + \
    (!!((w) & (1ULL << 6))) + \
    (!!((w) & (1ULL << 7)))))

#define __const_hweight16(w) (__const_hweight8(w)  + __const_hweight8((w)  >> 8 ))
#define __const_hweight32(w) (__const_hweight16(w) + __const_hweight16((w) >> 16))
#define __const_hweight64(w) (__const_hweight32(w) + __const_hweight32((w) >> 32))

/*
 * Generic interface.
 */
#define hweight8(w)  ( __const_hweight8(w)  )
#define hweight16(w) ( __const_hweight16(w) )
#define hweight32(w) ( __const_hweight32(w) )
#define hweight64(w) (__const_hweight64(w) )


static inline unsigned long hweight_long(unsigned long w)
{
  return sizeof(w) == 4 ? hweight32(w) : hweight64(w);
}

static inline void set_bit(int bit, volatile unsigned long *addr)
{
  unsigned long mask = BIT_MASK(bit);
  unsigned long *p = ((unsigned long *)addr) + BIT_WORD(bit);
  *p |= mask;
}

static inline void clear_bit(unsigned int bit, volatile unsigned long *addr)
{
  unsigned long mask = BIT_MASK(bit);
  unsigned long *p = ((unsigned long *)addr) + BIT_WORD(bit);
  *p &= ~mask;
}

static inline void change_bit(unsigned int bit, volatile unsigned long *addr)
{
  unsigned long mask = BIT_MASK(bit);
  unsigned long *p = ((unsigned long *)addr) + BIT_WORD(bit);
  *p ^= mask;
}

static inline int
test_bit(unsigned int bit, volatile unsigned long *addr)
{
  unsigned long mask = BIT_MASK(bit);
  unsigned long *p = ((unsigned long *)addr) + BIT_WORD(bit);
  return (*p & mask) != 0;
}

static inline int
test_and_set_bit(unsigned int bit, volatile unsigned long *addr)
{
  unsigned int res;
  unsigned long mask = BIT_MASK(bit);
  unsigned long *p = ((unsigned long *)addr) + BIT_WORD(bit);
  res = *p;
  *p = res | mask;
  return (res & mask) != 0;
}

static inline int
test_and_clear_bit(unsigned int bit, volatile unsigned long *addr)
{
  unsigned int res;
  unsigned long mask = BIT_MASK(bit);
  unsigned long *p = ((unsigned long *)addr) + BIT_WORD(bit);
  res = *p;
  *p = res & ~mask;
  return (res & mask) != 0;
}

static inline int
test_and_change_bit(unsigned int bit, volatile unsigned long *addr)
{
  unsigned int res;
  unsigned long mask = BIT_MASK(bit);
  unsigned long *p = ((unsigned long *)addr) + BIT_WORD(bit);
  res = *p;
  *p = res ^ mask;
  return (res & mask) != 0;
}

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 *
 * Returns the bit number for the next set bit
 * If no bits are set, returns @size.
 */
extern unsigned long find_next_bit(const unsigned long *addr, unsigned long
                                   size, unsigned long offset);
/**
 * find_next_zero_bit - find the next cleared bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 *
 * Returns the bit number of the next zero bit
 * If no bits are zero, returns @size.
 */
extern unsigned long find_next_zero_bit(const unsigned long *addr, unsigned
                                        long size, unsigned long offset);


/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum number of bits to search
 *
 * Returns the bit number of the first set bit.
 * If no bits are set, returns @size.
 */
extern unsigned long find_first_bit(const unsigned long *addr,
                                    unsigned long size);

/**
 * find_first_zero_bit - find the first cleared bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum number of bits to search
 *
 * Returns the bit number of the first cleared bit.
 * If no bits are zero, returns @size.
 */
extern unsigned long find_first_zero_bit(const unsigned long *addr,
    unsigned long size);


#define for_each_set_bit(bit, addr, size) \
  for ((bit) = find_first_bit((addr), (size));    \
       (bit) < (size);          \
       (bit) = find_next_bit((addr), (size), (bit) + 1))

/* same as for_each_set_bit() but use bit as value to start with */
#define for_each_set_bit_from(bit, addr, size) \
  for ((bit) = find_next_bit((addr), (size), (bit));  \
       (bit) < (size);          \
       (bit) = find_next_bit((addr), (size), (bit) + 1))

#define for_each_clear_bit(bit, addr, size) \
  for ((bit) = find_first_zero_bit((addr), (size)); \
       (bit) < (size);          \
       (bit) = find_next_zero_bit((addr), (size), (bit) + 1))

/* same as for_each_clear_bit() but use bit as value to start with */
#define for_each_clear_bit_from(bit, addr, size) \
  for ((bit) = find_next_zero_bit((addr), (size), (bit)); \
       (bit) < (size);          \
       (bit) = find_next_zero_bit((addr), (size), (bit) + 1))


/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
 * @size: The number of bits to search
 *
 * Returns the bit number of the last set bit, or size.
 */
extern unsigned long find_last_bit(const unsigned long *addr,
                                   unsigned long size);
#endif