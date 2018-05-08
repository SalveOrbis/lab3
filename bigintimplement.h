
/*********************************************************************
 * The following functions must be implemented to solve the          *
 * problems in the laboration.                                       *
 *********************************************************************/

/**
 * Initializes this input.
 */
void bi_init(bi_t bi);

/**
 * Deallocates the memory allocated in the initialized input.
 */
void bi_clear(bi_t bi);

/**
 * Prints a to stdout as a hexadecimal integer.
 */
void bi_printf(bi_t a);

/**
 * Sets bi equal to the integer represented by the hexadecimal string
 * given as input.
 */
void bi_import(bi_t res, char *s);

/**
 * Writes a signed hexadecimal representation of the value of a to
 * res. It is assumed that res has sufficient space. The number of
 * characters written is returned.
 */
int bi_export(char *res, bi_t a);

/**
 * Sets res = a.
 */
void bi_set(bi_t res, bi_t a);

/**
 * Sets res = u, where u is an unsigned WORDSIZE-bit integer.
 */
void bi_setui(bi_t res, unsigned int u);

/**
 * Sets res equal to a random non-negative integer with the given
 * nominal number of bits. Keep in mind that each limb must contain at
 * most WORDSIZE non-zero bits and they should be the least
 * significant bits.
 *
 * The most significant word requires special attention, since some of
 * its bits may have to be set to zero if bits is not divisible by
 * WORDSIZE.
 *
 * We stress that some (or even all!) leading bits in the most
 * significant word may be zero, since they are randomly chosen.
 *
 * We are using this random generator for testing purposes, so we do
 * not need to use any cryptographically strong source such as
 * /dev/urandom. It is perfectly fine to use rand() from stdlib.h, but
 * keep in mind that it outputs a number in a small interval.
 */
void bi_rand(bi_t res, int bits);

/**
 * Returns the number of bits needed to represent a->value as an
 * unsigned integer.
 */
int bi_bitsize(bi_t a);

/**
 * Returns the ith bit as an integer.
 */
int bi_tstbit(bi_t a, int i);

/**
 * Returns -1, 0, or 1 depending on if a is smaller, equal, or greater
 * than b as signed integers.
 */
int bi_cmp(bi_t a, bi_t b);

/**
 * Sets res = -a.
 */
void bi_neg(bi_t res, bi_t a);

/**
 * Sets res = a + b.
 */
void bi_add(bi_t res, bi_t a, bi_t b);

/**
 * Sets res = a - b.
 */
void bi_sub(bi_t res, bi_t a, bi_t b);

/**
 * Sets res = a * b.
 */
void bi_mul(bi_t res, bi_t a, bi_t b);

/**
 * Sets res = a ^ b. If one input is longer then the other is
 * considered to contain the needed number of leading zero to do the
 * operation.
 */
void bi_xor(bi_t res, bi_t a, bi_t b);

/**
 * Sets res = a & b. If one input is longer then the other is
 * considered to contain the needed number of leading zero to do the
 * operation.
 */
void bi_and(bi_t res, bi_t a, bi_t b);

/**
 * Sets res = a | b. If one input is longer then the other is
 * considered to contain the needed number of leading zero to do the
 * operation.
 */
void bi_or(bi_t res, bi_t a, bi_t b);
