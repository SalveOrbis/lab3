
/*********************************************************************
 * The following can be implemented as subroutines to help implement *
 * the above. This gives a naive implementation.                     *
 *********************************************************************/

/**
 * Reduces the number of limbs in bi such that the leading limb of
 * bi->value is non-zero, except if bi == 0 in which case we set
 * bi->limbs = 1 to be able to store zero.
 */
void bi_normalize(bi_t bi);

/**
 * Set all limbs in res->value starting at index i to zero without
 * changing res->limbs.
 */
void bi_setzero(bi_t res, int i);

/**
 * Generates a pseudo-random word.
 */
int bi_randword();

/**
 * Resizes bi->value to have limbs number of limbs and sets a->value
 * to the all-zero array and the sign to zero.
 */
void bi_resize(bi_t bi, int limbs);

/**
 * Applies op limb-wise to limbs of a and b and stores the result in
 * res. Here op is a function pointer, so any function with the given
 * signature can be used. (This is convenient for this problem, but
 * far too slow in a real implementation.)
 */
void bi_map(bi_t res, bi_t a, bi_t b, int (*op)(int, int));

/**
 * xor as a function.
 */
int bi_ixor(int x, int y);

/**
 * and as a function.
 */
int bi_iand(int x, int y);

/**
 * or as a function.
 */
int bi_ior(int x, int y);

/**
 * Returns the integer value of the input hexadecimal character.
 */
int hex_to_int(char c);

/**
 * Returns the hexadecimal character corresponding to the integer
 * input.
 */
char int_to_hex(int h);

/**
 * Returns the ith block of a->value.
 */
int bi_getblk(bi_t a, int i);

/**
 * Sets the ith block of a->value.
 */
int bi_setblk(bi_t a, int i, int blk);

/**
 * Returns the number of blocks in a->value.
 */
int bi_blocks(bi_t a);

/**
 * Returns -1, 0, or 1 depending on if a->value is smaller, equal, or
 * greater than b->value as unsigned integers.
 */
int bi_ucmp(bi_t a, bi_t b);

/**
 * Sets res->value = a->value + b->value without setting res->sign or
 * changing res->limbs.
 *
 * ASSUMES: res->limbs >= max(a->limbs, b->limbs). If equality holds,
 * then the overflow is discarded.
 */
void bi_uadd(bi_t res, bi_t a, bi_t b);

/**
 * Sets res->value = - a->value in two's complement for integers with
 * res->limbs * WORDSIZE bits.
 */
void bi_pneg(bi_t res, bi_t a);

/**
 * Sets res->value = a->value - b->value. This function assumes that
 * a->value > b->value.
 */
void bi_usub(bi_t res, bi_t a, bi_t b);

/**
 * Sets res->value = |a->value - b->value| and sets res->sign equal to
 * 1, 0, or -1 depending on if a->value is greater, equal, or smaller
 * than b->value.
 */
void bi_uabsdiff(bi_t res, bi_t a, bi_t b);

/**
 * Sets res->value = a->value + b->value * scalar * 2^(shift * WORDSIZE)
 * mod 2^((a->limbs + shift) * WORDSIZE).
 */
void bi_umuladd(bi_t res, bi_t a, int scalar, int shift);
