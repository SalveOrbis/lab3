#include "bigint.h"


/*********************************************************************
 * The following can be implemented as subroutines to help implement *
 * the above. This gives a naive implementation.                     *
 *********************************************************************/

/**
 * Reduces the number of limbs in bi such that the leading limb of
 * bi->value is non-zero, except if bi == 0 in which case we set
 * bi->limbs = 1 to be able to store zero.
 */
void bi_normalize(bi_t bi) {
  int sign = bi->sign;
  int new_length;
  int i = bi->limbs - 1 ;
  //Leta efter första signifikanta siffran
  while (i > 0 && bi->value[i] == 0) {
    i--;
  }
  new_length = i+1; 

  if (new_length < bi->limbs) {
    //Kopiera gamla värdet till vektor med rätt storlek
    int *copy = malloc(new_length*LIMBBYTES);
    for (int j = 0; j < new_length; j++) {
      copy[j] = bi->value[j];
    }

    bi_resize(bi, new_length);
    for (int j = 0; j < new_length; j++) {
       bi->value[j] = copy[j] ;
    }

    free(copy); //kasta bort kopian
    if( bi->limbs == 1 && bi->value[0] == 0) {
      bi->sign = 0;
    }
    else {
      bi->sign = sign;

  }
}
  else if (new_length == bi->limbs) {

  }
  else {
    // bi = 0
    bi_resize(bi, 1);
    bi->limbs = 1;
    bi->sign = 0;
    bi->value[0] = 0;
  }
}

/**
 * Set all limbs in res->value starting at index i to zero without
 * changing res->limbs.
 */
void bi_setzero(bi_t res, int i) {
  for ( i ; i < res->limbs ; i++) {
    res->value[i] = 0;
  }
}

/**
 * Generates a pseudo-random word.
 */
int bi_randword() {
  int randomword = arc4random();
  randomword = randomword & WORDMASK;
  return randomword;
}

/**
 * Resizes bi->value to have limbs number of limbs and sets a->value
 * to the all-zero array and the sign to zero.
 */
void bi_resize(bi_t bi, int limbs) {
  bi_clear(bi);

  bi->value = malloc (LIMBBYTES * limbs); //frigör nytt  utrymme
  bi->sign = 0;
  bi->limbs = limbs;

  //Nollställ alla limbs
  for (int i = 0; i < limbs ; i++) {
    bi->value[i] = 0;
  } 

}

/**
 * Applies op limb-wise to limbs of a and b and stores the result in
 * res. Here op is a function pointer, so any function with the given
 * signature can be used. (This is convenient for this problem, but
 * far too slow in a real implementation.)
 */
void bi_map(bi_t res, bi_t a, bi_t b, int (*op)(int, int)) {
}

/**
 * xor as a function.
 */
int bi_ixor(int x, int y) {
  return x ^ y;
}

/**
 * and as a function.
 */
int bi_iand(int x, int y) {
  return x & y;
}

/**
 * or as a function.
 */
int bi_ior(int x, int y) {
  return x | y;
}

/**
 * Returns the integer value of the input hexadecimal character.
 */
int hex_to_int(char c) {
    int val;
    if (c == '1') {
      val = 1; 
    } else if (c == '2') {
      val = 2;
    } else if (c == '3') {
      val = 3;
    } else if (c == '4') {
      val = 4;
    } else if (c == '5') {
      val = 5;
    } else if (c == '6') {
      val = 6;
    } else if (c == '7') {
      val = 7;
    } else if (c == '8') {
      val = 8;
    } else if (c == '9') {
      val = 9;
    } else if (c == 'a' || c == 'A') {
      val = 10;
    } else if (c == 'b' || c == 'B') {
      val = 11;
    } else if (c == 'c' || c == 'C') {
      val = 12;
    } else if (c == 'd' || c == 'D') {
      val = 13;
    } else if (c == 'e' || c == 'E') {
      val = 14;
    } else if (c == 'f' || c == 'F') {
      val = 15;
    } else {
      val = 0;
    }
    
  return val;
}

/**
 * Returns the hexadecimal character corresponding to the integer
 * input.
 */
char int_to_hex(int h) {
   char  hex;
    if (h == 1) {
      hex = '1'; 
    } else if (h == 2) {
      hex = '2';
    } else if (h == 3) {
      hex = '3';
    } else if (h == 4) {
      hex = '4';
    } else if (h == 5) {
      hex = '5';
    } else if (h == 6) {
      hex = '6';
    } else if (h == 7) {
      hex = '7';
    } else if (h ==8) {
      hex = '8';
    } else if (h == 9) {
      hex = '9';
    } else if (h == 10) {
      hex = 'a';
    } else if (h == 11) {
      hex = 'b';
    } else if (h == 12) {
      hex = 'c';
    } else if (h == 13) {
      hex = 'd';
    } else if (h == 14) {
      hex = 'e';
    } else if (h == 15) {
      hex = 'f';
    } else {
      hex = '0';
    }
    
  return hex;
}

/**
 * Returns the ith block of a->value.
 */ 
//första blocket har index 0
int bi_getblk(bi_t a, int i) {
  //Vilken limb finns blocket i
  int limb_index = i / BLOCKS;

  int skiftningar = i*BLOCKSIZE - (WORDSIZE)*limb_index;
  int word = a->value[limb_index];
  word >>= skiftningar;  

  int block = word & BLOCKMASK;

  return block;
}

/**
 * Sets the ith block of a->value.
 */
int bi_setblk(bi_t a, int i, int blk) {
  int limb_index = i / BLOCKS;
  int block_value = blk << (i*BLOCKSIZE - BLOCKS*(limb_index)*BLOCKSIZE);
  int mask = BLOCKMASK <<(i*BLOCKSIZE - BLOCKS*(limb_index)*BLOCKSIZE);
  mask =  ~mask;
  a->value[limb_index] &= mask;
  
   a->value[limb_index] += block_value;
  return 0;
}

/**
 * Returns the number of blocks in a->value.
 */
int bi_blocks(bi_t a) {
   //Specialfall för a = 0
  if( a->limbs == 1 && a->value == 0 ) {
    return 1;
  }

  //börja med mest signifikanta blocket, fortsätt kolla till blocket != 0
  int antal_block = (a->limbs * BLOCKS) ;
  int limb_index = a->limbs -1;
  int top_word = a->value[limb_index];

  int antal_skiftningar = 0;
  int mask = BLOCKMASK << BLOCKSIZE *5;  

  while ( !(top_word & mask) && antal_skiftningar < BLOCKS-1 ) {
      antal_skiftningar++;
      top_word <<=  BLOCKSIZE;
      antal_block-- ;
  }

 
  return antal_block;
}

/**
 * Returns -1, 0, or 1 depending on if a->value is smaller, equal, or
 * greater than b->value as unsigned integers.
 */
int bi_ucmp(bi_t a, bi_t b) {
  if (a->limbs > b->limbs) {
    return 1;
  } 
  else if (a->limbs < b->limbs) {
    return -1;
  }
  else {
    int limb_index = a->limbs -1;
    while (a->value[limb_index] == b->value[limb_index] && limb_index > 0) {
      limb_index--;
    }
    if (a->value[limb_index] > b->value[limb_index]) {
      return 1;
    } else if (a->value[limb_index] < b->value[limb_index]) {
      return -1;
    } else {
      return 0;
    }

  }

  return 0;
}

/**
 * Sets res->value = a->value + b->value without setting res->sign or
 * changing res->limbs.
 *
 * ASSUMES: res->limbs >= max(a->limbs, b->limbs). If equality holds,
 * then the overflow is discarded.
 */
void bi_uadd(bi_t res, bi_t a, bi_t b) {
  // printf("BÖRJAN AV UADD\n");
  int min_limbs = MIN(a->limbs, b->limbs);
  int max_limbs = MAX(a->limbs, b->limbs);
  int carry_over = 0;
  // printf("\na->limbs: %d, b->limbs: %d\n", a->limbs, b->limbs);

  int *biggest_int ; //ska peka på bi_t->value som har flest limbs
  
  if (a->limbs == max_limbs) {
        biggest_int = a->value;
  } else {
 
        biggest_int = b->value;
  }

  int i = 0;
  for (  i = 0; i < min_limbs; i++) {
    res->value[i] = a->value[i] + b->value[i] + carry_over;
    carry_over = check_carry(res->value[i]);
    res->value[i] &= WORDMASK; //maskera bort nails
  }



  if ( min_limbs != max_limbs) { //fallet där ett tal har fler limbs
    for (i; i < max_limbs ; i++) {
      res->value[i] = biggest_int[i] + carry_over;
      carry_over = check_carry(res->value[i]);
      res->value[i] &= WORDMASK; //maskera bort nails
      
    }
  }

  if (carry_over == 1) {
    res->value[res->limbs-1] += 1; //flytta carry-over till sista limben i res
  }
}




//returnera 1 om vi har en carry-over, 0 annars
int check_carry(int limb) {
  if (limb > WORDMASK) {
    return 1;
  } else {
    return 0;
  }
}

//returnera 1 om vi ska låna av en limb
int check_borrow (int res, int a_limbs, int i) {
  if ( res < 0  && a_limbs > i+1 ) {
    return 1;

  }
  else {
    return 0;
  }

}


/**
 * Sets res->value = - a->value in two's complement for integers with
 * res->limbs * WORDSIZE bits.
 */
void bi_pneg(bi_t res, bi_t a) {
}

/**
 * Sets res->value = a->value - b->value. This function assumes that
 * a->value > b->value.
 */
void bi_usub(bi_t res, bi_t a, bi_t b) {
  int min_limbs = MIN(a->limbs, b->limbs);
  int max_limbs = MAX(a->limbs, b->limbs);

  int *biggest_int ; //ska peka på bi_t->value som har flest limbs
  
  if (a->limbs == max_limbs) {
        biggest_int = a->value;
  } else {
 
        biggest_int = b->value;
  }

  int i;
  int borrow = 0 ;
  int has_borrowed = 0;
  int resultat ;
  for ( i = 0 ; i < min_limbs; i++)
  {
    resultat = a->value[i] - b->value[i] - has_borrowed;
    has_borrowed = check_borrow(resultat, a->limbs, i);
    resultat  = (has_borrowed == 1) ? resultat + 1 + WORDMASK : resultat;
    res->value[i] = resultat & WORDMASK;
  }

  if (max_limbs != min_limbs) {
    for ( i; i < max_limbs; i++)
    {
      
      resultat = biggest_int[i] - has_borrowed;
      has_borrowed = check_borrow(resultat, a->limbs, i);
      borrow = (has_borrowed == 1) ? WORDMASK : 0;
      resultat = resultat + borrow;
      res->value[i] = resultat & WORDMASK;
    }


  }



}

/**
 * Sets res->value = |a->value - b->value| and sets res->sign equal to
 * 1, 0, or -1 depending on if a->value is greater, equal, or smaller
 * than b->value.
 */
void bi_uabsdiff(bi_t res, bi_t a, bi_t b) {

  int min_limbs = MIN(a->limbs, b->limbs);
  int max_limbs = MAX(a->limbs, b->limbs);

  int *biggest_int ; //ska peka på bi_t->value som har flest limbs
  
  if (a->limbs == max_limbs) {
        biggest_int = a->value;
  } else {
 
        biggest_int = b->value;
  }

  int i;
  int borrow = 0 ;
  int has_borrowed = 0;
  int resultat ;
  for ( i = 0 ; i < min_limbs; i++)
  {
    resultat = a->value[i] - b->value[i] - has_borrowed;
    has_borrowed = check_borrow(resultat, a->limbs, i);
    borrow = (has_borrowed == 1) ? WORDMASK : 0;
    resultat = resultat + borrow;
    resultat = abs(resultat);
    res->value[i] = resultat & WORDMASK;
  }

  if (max_limbs != min_limbs) {
    for ( i; i < max_limbs; i++)
    {
      
      resultat = biggest_int[i] - has_borrowed;
      has_borrowed = check_borrow(resultat, a->limbs, i);
      borrow = (has_borrowed == 1) ? WORDMASK : 0;
      resultat = resultat + borrow;
      resultat = abs(resultat);
      res->value[i] = resultat & WORDMASK;
    }


  }
  

    // TECKENHANTERING
    if (bi_ucmp(a,b) == 1 ) {
      if (a->sign == 1) {
         res->sign = 1;
      } else {
        res->sign = -1;
      }
  }
   else if (bi_ucmp(a,b) == 0) {
    res->sign = 0;
  } 
  else if (bi_ucmp(a,b) == -1) {
    if (a->sign == 1) {
      res->sign = -1;
      }
      else {
        res->sign = 1;
      }
  }

}

/**
 * Sets res->value = a->value + b->value * scalar * 2^(shift * WORDSIZE)
 * mod 2^((a->limbs + shift) * WORDSIZE).
 */

// 2^(shift * WORDSIZE) skiftar skift antal bitarna ett ord


void bi_umuladd(bi_t res, bi_t a,  int scalar, int shift) {

  int resultat;
  for (int i = 0  ; i < a->limbs; i++) {
     resultat = a->value[i] +  scalar * (int)pow(2, shift * WORDSIZE);
     resultat %= (int) pow(2, (a->limbs + shift) * WORDSIZE); 
     res->value[i] = resultat;
  }
}

void bi_mulen (bi_t res, bi_t a, int b_limb, int shift) {
  unsigned long long resultat;
  int i ;
  int carry = 0;
  for ( i = 0; i < a->limbs; ++i)
  {
    // printf("res-value sen tidigare: %x\n",res->value[i+shift] );
    resultat =  (unsigned long long)a->value[i] *(unsigned long long) b_limb; // + carry + res->value[i+shift];
    resultat += (unsigned long long) carry + (unsigned long long)res->value[i+shift];
    // printf("res: %x \n", resultat);
    //kolla om vi får en carry till nästa limb
    carry = mul_carry (resultat); 
    res->value[i+shift] =  ( resultat & (long long) WORDMASK);
  }

  if (carry > 0) {
    res->value[i+shift] = carry;
  }

}

int mul_carry ( unsigned long long res) {

  // printf("Inne i mul_carry\n");
  // printf("res: %x \n", res );
  // long long biggest_word = 0xFFFFFF;
  unsigned long skiftad = (unsigned long)res >> WORDSIZE; 

  // printf("skiftad: %x \n", skiftad );

  if (skiftad > 0) {
  // if (res > biggest_word) {
    // long long  carry =  res >> (long long) WORDSIZE;
    // printf("\ncarry: %x\n", skiftad);
    return (int) skiftad;
  } else {
    // printf("Det är mindre\n");
    return 0;
  }
}