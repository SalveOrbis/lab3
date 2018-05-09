#include "bigint.h"
#include <stdio.h>
#include <stdlib.h>

/*********************************************************************
 * The following functions must be implemented to solve the          *
 * problems in the laboration.                                       *
 *********************************************************************/

/**
 * Initializes this input.
 */
void bi_init(bi_t bi) {
	bi->sign = 0;
	bi->value = malloc (sizeof(*(bi->value) * DEFAULT_LIMBS)); //@LAB3
	bi->limbs = DEFAULT_LIMBS;
	for (int i = 0;  i < DEFAULT_LIMBS; i++ ) { 
		bi->value[i] = 0;
	}

}

/**
 * Deallocates the memory allocated in the initialized input.
 */
void bi_clear(bi_t bi) {
	free(bi->value);
}

/**
 * Prints a to stdout as a hexadecimal integer.
 */
void bi_printf(bi_t a) {
	if (a->sign == -1) {
		printf("-");
	}

	int antal_block = bi_blocks(a);
	for (int i = antal_block-1; i >= 0; i-- ) {
		int block = bi_getblk(a, i);
		char hexdec = int_to_hex(block);
		printf("%c", hexdec);
	}

}

/**
 * Sets bi equal to the integer represented by the hexadecimal string
 * given as input.
 */
void bi_import(bi_t res, char *s) {
	int i = 0;
	int antal_block = 0;
	int sign = 1;
	if (s[i] == '-') {
		sign = -1;
		// printf("hittade ett minus\n");
		i++;
	}

	// printf("import \n");
	while(s[i] != '\0') {
		// printf("%c", s[i]);
		i++;
		++antal_block; 
	}

	// antal_block = 
	// printf("\n");
	// printf("antal chars: %d\n i: %d", antal_block, i);

	bi_resize(res, 1 + antal_block/BLOCKS);
	int block_index = antal_block-1;
	// i = (res->sign == -1) ?  1 : 0 ;

	if (sign == -1) {
		res->sign = -1;
		i = 1;
	} else
	{
		res->sign = 1;
		i = 0;
	}


	for (block_index; block_index >= 0; block_index--) {
		char hexdec = s[i++];
		// printf("\nhex: %c\n", hexdec);
		int blocket = hex_to_int (hexdec);
		bi_setblk(res, block_index, blocket); 
	}

	// printf("i: %d\n", i );
	bi_normalize(res);

	if (sign == -1) {
		res->sign = -1;
	} else
	{
		res->sign = 1;
	}

}

/**
 * Writes a signed hexadecimal representation of the value of a to
 * res. It is assumed that res has sufficient space. The number of
 * characters written is returned.
 */
int bi_export(char *res, bi_t a) {
	int antal_block = bi_blocks(a);
	// printf("\nantal block: %d\n", antal_block);
	int i = 0;

	if (a->sign == -1) {
		res[i] = '-';
		i++;
	}

	int block_index = antal_block-1 ;

	// printf("\n export:\n" );

	for ( block_index ; block_index >= 0; block_index--) {
		int blocket = bi_getblk(a, block_index);
		char hexdec = int_to_hex(blocket);
		// printf("%c",hexdec );
		res[i++] = hexdec;
	}
	// printf("\n");
	
	res[i] = '\0';
	// printf("\n strängen: \n%s\n", res );

  return --i;
}

/**
 * Sets res = a.
 */
void bi_set(bi_t res, bi_t a) {
	bi_clear(res);
	*res = *a;
}

/**
 * Sets res = u, where u is an unsigned WORDSIZE-bit integer.
 */
void bi_setui(bi_t res, unsigned int u) {
}

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
void bi_rand(bi_t res, int bits) {
	int antal_bitar = bits;
	int antal_limbs = antal_bitar / WORDSIZE + 1;
	if (antal_bitar %  WORDSIZE == 0 && antal_bitar != 0) {
		antal_limbs--;
	}

	// printf("antal limbs: %d\n", antal_limbs );
	bi_resize(res, antal_limbs);


	for (int i = 0; i < antal_limbs; ++i)
	{
		int word = bi_randword();
		res->value[i] = word;
	}

	//Skifta  bort bitar i översta limben
	int antal_skiftningar =  (antal_limbs)*WORDSIZE - antal_bitar;
	int last_limb = res->value[antal_limbs -1];

	last_limb >>= antal_skiftningar;
	res->value[antal_limbs -1] = last_limb;



	bi_normalize(res);
	res->sign = 1;
}

/**
 * Returns the number of bits needed to represent a->value as an
 * unsigned integer.
 */
int bi_bitsize(bi_t a) {
	int antal_block = bi_blocks(a); 

	//kolla bitar i sista blocket
	int block_index = antal_block-1;
	int mask = 8; //Signifikanta biten i ett 4 bitars tal
	int blocket = bi_getblk(a, block_index);
	int antal_bitar = BLOCKSIZE;
	while ((blocket & mask) == 0 && antal_bitar > 0 ) {
		antal_bitar--;
		mask >>= 1;
		// printf("\nantal bitar räknade: %d\n", antal_bitar);
	}

	antal_bitar += (antal_block-1) * BLOCKSIZE;

  return antal_bitar;
}

/**
 * Returns the ith bit as an integer.
 */
int bi_tstbit(bi_t a, int i) {
  return 0;
}

/**
 * Returns -1, 0, or 1 depending on if a is smaller, equal, or greater
 * than b as signed integers.
 */
int bi_cmp(bi_t a, bi_t b) {
	if (a->sign > b->sign ) {
		// printf("a större än b \n");
		// printf("tecken a: %d, tecken b: %d \n", a->sign, b->sign);
		return 1;
	} 
	else if (a->sign < b->sign) {
		// printf("b större än a \n");
		return -1;
	}
	else {
		// printf("a = b \n");
		// printf("tecken a: %d, tecken b: %d \n", a->sign, b->sign);
		if (a->sign < 0) {
			if (bi_ucmp(a,b) == 1) { //a är mindre
				return -1;
			 } else if (bi_ucmp(a,b) == -1) {
			 	return 1;
			 } else {
			 	return 0;
			 }
		}
		else if (a->sign > 0) {
			if (bi_ucmp(a,b) == 1) { //a är större
				return 1;
			 } else if (bi_ucmp(a,b) == -1) {
			 	return -1;
			 } else {
			 	return 0;
			 }
		}
		else if (a->sign == 0) {
			return 0;
		}

	}
  return 0;
}

/**
 * Sets res = -a.
 */
void bi_neg(bi_t res, bi_t a) {
}

/**
 * Sets res = a + b.
 */
void bi_add(bi_t res, bi_t a, bi_t b) {
}

/**
 * Sets res = a - b.
 */
void bi_sub(bi_t res, bi_t a, bi_t b) {
}

/**
 * Sets res = a * b.
 */
void bi_mul(bi_t res, bi_t a, bi_t b) {
}

/**
 * Sets res = a ^ b. If one input is longer then the other is
 * considered to contain the needed number of leading zero to do the
 * operation.
 */
void bi_xor(bi_t res, bi_t a, bi_t b) {
}

/**
 * Sets res = a & b. If one input is longer then the other is
 * considered to contain the needed number of leading zero to do the
 * operation.
 */
void bi_and(bi_t res, bi_t a, bi_t b) {
}

/**
 * Sets res = a | b. If one input is longer then the other is
 * considered to contain the needed number of leading zero to do the
 * operation.
 */
void bi_or(bi_t res, bi_t a, bi_t b) {
}
