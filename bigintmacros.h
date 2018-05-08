
/*********************************************************************
 * The following macros and declarations are used by the             *
 * testing framework and must not be changed.                        *
 *********************************************************************/

/* Number of bytes in a limb. If you have paid attention to lectures
   you know that this may give a different value on different
   platforms, since C does not define the size of an "int". */
#define LIMBBYTES (sizeof(int))

/* Number of bits in a word. This must be a multiple of 4. */
#define WORDSIZE 24

/* Number of bits in a limb that does not belong to the word. */
#define NAILS (8 * LIMBBYTES - WORDSIZE)

/* A word where all bits are set. */
#define WORDMASK ((1 << WORDSIZE) - 1)

/* A word with the most significant bit set. */
#define TOPMASK (1 << (WORDSIZE - 1))

/* Number of bits in a block. */
#define BLOCKSIZE 4

/* Number of 4-bit blocks in a word. */
#define BLOCKS (WORDSIZE / BLOCKSIZE)

/* A word where the four least significant bits are set, i.e., 15 or
   in hexadecimal 0xF. */
#define BLOCKMASK ((1 << BLOCKSIZE) - 1)

/* Default number of limbs allocated in an initialized instance. */
#define DEFAULT_LIMBS 1

/* Note that we encapsulate each value with parenthesis. This is due
   to how macros work. Note also that these macros are not equivalent
   to their corresponding implementations as functions due to side
   effects. Consider the value of MAX(1++,3) for example. */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * Structure representing a signed integer. It has a sign {-1, 0, 1},
 * a value (array of limbs containing words), and the number of
 * limbs/words.
 *
 * We use the fixed-size array trick to get poor man's
 * pass-by-reference in C similar to C++ way of using references
 * automatically and cleaner interfaces.
 *
 * The idea is to use the fact that C passes arrays by reference, so
 * we put a struct in an array with a single element.
 */
typedef struct bi_t {
  int sign;
  int *value;
  int limbs;
} bi_t[1];
