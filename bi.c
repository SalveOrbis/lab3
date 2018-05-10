
/*********************************************************************
 * This code is used to test your library.                           *
 *                                                                   *
 * The overall strategy in these tests is to rely on GMP to be       *
 * be correct. We generate suitable integers and then run the same   *
 * operations both using our own implementation and using GMP and    *
 * then we compare the results.                                      *
 *                                                                   *
 * This is a common approach even when there is nothing to use for   *
 * comparison. In that case, one may first write a simplistic        *
 * implementation that does only what is needed to verify that basic *
 * functionality is implemented correctly. Then one can use the      *
 * program itself for testing. For example, to test that addition    *
 * and subtraction is implemented correctly, one can generate random *
 * a and b and then verify that (a + b) - b = a. It turns out that   *
 * very little is needed to start from if one is careful!            *
 *                                                                   *
 * For our strategy to work we need to be able to convert our        *
 * representations of integers to that used by GMP, but this amounts *
 * to little resizing and invoking routines of GMP.                  *
 *********************************************************************/

#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "bigint.h"
#include "correct.h"

int verbose = 0;
void eget_test(void);

/**
 * Print error message to stderr.
 */
void error(char *msg) {
  fprintf(stderr, "ERROR: %s", msg);
  exit(1);
}

/**
 * Print start of complex error message.
 */
void error_head(char *msg) {
  fprintf(stderr, "\n\nERROR: %s", msg);
}

/**
 * Print end of complex error message.
 */
void error_tail() {
  fprintf(stderr, "\n");
  exit(1);
}

/**
 * Indicates start of test.
 */
void test_head(char *msg) {
  if (verbose) {
    printf("Testing %s... ", msg);
  }
}

/**
 * Indicates end of test.
 */
void test_tail() {
  if (verbose) {
    printf("done.\n");
  } else {
    printf("%lx\n", CORRECT);
  }
}

/**
 * Convert an instance of bi_t to an instance of mpz_t representing
 * the same integer value.
 */
void bi_to_mpz(mpz_t res, bi_t a) {
  mpz_import(res, a->limbs, -1, LIMBBYTES, 0, NAILS, a->value);
  if (a->sign < 0) {
    mpz_neg(res, res);
  }
}

/**
 * Convert an instance of mpz_t to an instance of bi_t representing
 * the same integer value.
 */
void mpz_to_bi(bi_t res, mpz_t a) {
  bi_resize(res, (mpz_sizeinbase(a, 1 << BLOCKSIZE) + BLOCKS - 1) / BLOCKS);
  mpz_export (res->value, NULL, -1, LIMBBYTES, 0, NAILS, a);
  res->sign = mpz_sgn(a);
  bi_normalize(res);
}

/**
 * Display the name and value of a bi_t instance and a newline. This
 * is used for error output.
 */
void bi_disp(char *name, bi_t a) {
  printf("%s = ", name);
  bi_printf(a);
  printf("\n");
}

/**
 * Display the name and value of a mpz_t instance and a newline. This
 * is used for error output.
 */
void mpz_disp(char *name, mpz_t a) {
  gmp_fprintf(stdout, "%s = %Zx\n", name, a);
}

/**
 * Test conversion between bi_t and mpz_t instances.
 */
void bi_test_conv() {
  bi_t a;
  bi_t b;
  mpz_t x;
  int i;

  bi_init(a);
  bi_init(b);
  mpz_init(x);

  test_head("bi_to_mpz/mpz_to_bi");

  for (i = 1; i < 101; i++) {

    bi_rand(a, i);
    bi_to_mpz(x, a);
    mpz_to_bi(b, x);

    if (bi_cmp(a, b) != 0) {
      error("Failed to convert!");
    }
  }

  test_tail();

  bi_clear(a);
  bi_clear(b);
  mpz_clear(x);
}

/**
 * Test that bitsizes are computed correctly.
 */
void bi_test_bitsize() {
  bi_t a;
  mpz_t x;
  int i;
  int ba;
  int bx;

  bi_init(a);
  mpz_init(x);

  test_head("bi_bitsize");

  for (i = 1; i < 101; i++) {

    bi_rand(a, i);
    bi_to_mpz(x, a);

    ba = bi_bitsize(a);
    bx = mpz_sizeinbase(x, 2);

    if (ba != bx) {
      error("Bad bitsize!");
    }
  }

  test_tail();

  bi_clear(a);
  mpz_clear(x);
}

/**
 * Test that reading an addressed bit works.
 */
void bi_test_tstbit() {
  bi_t a;
  mpz_t x;
  int i;
  int j;
  int ba;
  int bx;

  bi_init(a);
  mpz_init(x);

  test_head("bi_tstbit");

  for (i = 1; i < 200; i++) {

    bi_rand(a, i);
    bi_to_mpz(x, a);

    ba = bi_bitsize(a);
    bx = mpz_sizeinbase(x, 2);

    for (j = 0; j < i + 10; j++) {
      if (bi_tstbit(a, j) != mpz_tstbit(x, j)) {
        fprintf(stderr, "%d\n", bi_tstbit(a, j));
        fprintf(stderr, "%d\n", mpz_tstbit(x, j));
        fprintf(stderr, "antal bitar: %d biten som läses fel: %d \n", i, j);

        error("Wrong bits!");
      }
    }
  }

  test_tail();

  bi_clear(a);
  mpz_clear(x);
}

/**
 * Test which verifies that the function bf implements the same
 * functionality for the specific inputs a and b as the corresponding
 * function zf in the GMP library, but using bi_t instances instead of
 * mpz_t instances.
 *
 * Note that if we can convert a and b to mpz_t instances a' and b'
 * correctly, then we can test multiple arithmetic operations using
 * the same code by effectively running our software and GMP
 * side-by-side.
 *
 * Here bf and zf are function pointers, i.e., an address to a
 * function that takes parameters of the given type. We could even
 * drop the parameter names for the functions bf and zf.
 */
void bi_para_v3(bi_t a, bi_t b,
                void (*bf)(bi_t res, bi_t a, bi_t b),
                void (*zf)(mpz_t zres, const mpz_t za, const mpz_t zb)) {
  
  bi_t res;
  mpz_t bres;

  mpz_t zres;
  mpz_t za;
  mpz_t zb;

  // Init
  bi_init(res);
  mpz_init(bres);

  mpz_init(zres);
  mpz_init(za);
  mpz_init(zb);

  // Convert inputs.
  bi_to_mpz(za, a);
  bi_to_mpz(zb, b);

  // Using bi-library.
  bf(res, a, b);
  bi_to_mpz(bres, res);

  // Using GMP.
  zf(zres, za, zb);


  if (mpz_cmp(bres, zres) != 0) {
    error_head("Failed to compute!\n");

    bi_disp("a", a);
    bi_disp("b", b);
    bi_disp("res", res);
    mpz_disp("bres", bres);

    mpz_disp("za", za);
    mpz_disp("zb", zb);
    mpz_disp("zres", zres);

    error_tail();
  }



  // Clear
  bi_clear(res);
  mpz_clear(bres);

  mpz_clear(zres);
  mpz_clear(za);
  mpz_clear(zb);

}

/**
 * Named test which verifies that the function bf implements the same
 * functionality as the corresponding function zf in the GMP library,
 * but using bi_t instances instead of mpz_t instances.
 *
 * Here bf and zf are function pointers, i.e., an address to a
 * function that takes parameters of the given type. We could even
 * drop the parameter names for the functions bf and zf.
 */
void bi_test_v3(char *name,
                void (*bf)(bi_t res, bi_t a, bi_t b),
                void (*zf)(mpz_t zres, const mpz_t za, const mpz_t zb)) {
  int i;
  int j;
  bi_t a;
  bi_t b;

  bi_init(a);
  bi_init(b);

  test_head(name);
  for (i = 1; i < 100; i++) {
    for (j = 1; j < 100; j++) {
      bi_rand(a, i);
      bi_rand(b, j);
      // fprintf(stderr,"i: %d j: %d\n",i,j );
      bi_para_v3(a, b, bf, zf);
    }
  }
  test_tail();

  bi_clear(a);
  bi_clear(b);
}

/**
 * Test comparison operator cmp.
 */
void bi_test_cmp() {
  int i;
  int j;
  int s;
  int t;
  int cmp;
  int zcmp;
  bi_t a;
  bi_t b;
  mpz_t za;
  mpz_t zb;

  bi_init(a);
  bi_init(b);
  mpz_init(za);
  mpz_init(zb);

  test_head("bi_cmp");
  for (i = 1; i < 100; i++) {
    for (j = 1; j < 100; j++) {
      for (s = 0; s < 2; s++) {
        for (t = 0; t < 2; t++) {

          bi_rand(a, i);
          if (s == 0) {
            bi_neg(a, a);
          }
          bi_rand(b, j);
          if (t == 0) {
            bi_neg(b, b);
          }

          // Convert inputs.
          bi_to_mpz(za, a);
          bi_to_mpz(zb, b);

          cmp = bi_cmp(a, b);
          zcmp = mpz_cmp(za, zb);

          // GMP does not use -1, 0, 1.
          zcmp = zcmp < 0 ? -1 : (zcmp > 0 ? 1 : 0);

          if (cmp != zcmp) {
            error_head("CMP Failed to compute!\n");

            bi_disp("a", a);
            bi_disp("b", b);
            printf("cmp = %d\n", cmp);
            printf("zcmp = %d\n", zcmp);

            error_tail();
          }
        }
      }
    }
  }
  test_tail();

  bi_clear(a);
  bi_clear(b);
  mpz_clear(za);
  mpz_clear(zb);
}

/**
 * Test import and export function.
 */
void bi_test_impexp() {
  bi_t a;
  bi_t b;
  int i;
  int t;
  char *s;

  bi_init(a);
  bi_init(b);

  test_head("bi_import/bi_export");

  for (i = 1; i < 1000; i++) {

    for (t = 0; t < 2; t++) {

      bi_rand(a, i);
      if (t == 0) {
        bi_neg(a, a);
      }

      // Make room for potential minus character.
      int antal_bitar = bi_bitsize(a);
      s = malloc(sizeof(*s) * ((antal_bitar + BLOCKSIZE - 1) / BLOCKSIZE + 2));

      bi_export(s, a);
      bi_import(b, s);

      if (bi_cmp(a, b) != 0) {
        error_head("IMPEXP Failed to compute!\n");

        bi_disp("a", a);
        printf("s = %s\n", s);
        bi_disp("b", b);
        printf("sa = %d\n", a->sign);
        printf("sb = %d\n", b->sign);
        printf("cmp = %d\n", bi_cmp(a, b));

        error_tail();
      }
      free(s);
    }
  }

  test_tail();

  bi_clear(a);
  bi_clear(b);
}

/**
 * Run the named test.
 */
void runtest(char *test_name) {
  if (strcmp(test_name, "bi_impexp") == 0) {
    bi_test_impexp();
  } else if (strcmp(test_name, "bi_add") == 0) {
    bi_test_v3("bi_add", bi_add, mpz_add);
  } else if (strcmp(test_name, "bi_sub") == 0) {
    bi_test_v3("bi_sub", bi_sub, mpz_sub);
  } else if (strcmp(test_name, "bi_mul") == 0) {
    bi_test_v3("bi_mul", bi_mul, mpz_mul);
  } else if (strcmp(test_name, "bi_and") == 0) {
    bi_test_v3("bi_and", bi_and, mpz_and);
  } else if (strcmp(test_name, "bi_or") == 0) {
    bi_test_v3("bi_or", bi_or, mpz_ior);
  } else if (strcmp(test_name, "bi_xor") == 0) {
    bi_test_v3("bi_xor", bi_xor, mpz_xor);
  } else if (strcmp(test_name, "bi_cmp") == 0) {
    bi_test_cmp();
  } else if (strcmp(test_name, "bi_bitsize") == 0) {
    bi_test_bitsize();
  } else if (strcmp(test_name, "bi_tstbit") == 0) {
    bi_test_tstbit();
  }
}

/**
 * Run the tests listed as lines on stdin.
 */
void runtests() {
  size_t size;
  char *test_name = malloc(sizeof(char) * 1024);

  while (getline(&test_name, &size, stdin) != -1) {
    test_name[strlen(test_name) - 1] = '\0';
    runtest(test_name);
  }
  free(test_name);
}

void eget_test(void) {
  bi_t a ;
  bi_init (a);
  bi_t b;
  bi_init (b);
  bi_t res;
  bi_init(res);
  

  // HÅRDKODADE VÄRDEN
  // int bitar = limbs * WORDSIZE;
  int limbs = 1;
  bi_resize(a, limbs);
  a->sign = 1;
  a->value[0] =  0x94f5;
   // a->value[2] =  WORDMASK;
  // a->value[2] =  10;
  // a->value[3] =  10;
  // a->value[4] =  10;
  // a->value[5] =  10;

  limbs = 2;
  bi_resize (b, limbs);
  b->sign = 2;
  b->value[0] = 0x008001;
  b->value[1] = 0xe920;
  // b->value[2] = 0x7bf25b;
  // b->value[1] = 1;
 
 //-e91fffeb0c

  // bi_normalize(a);
  // bi_normalize(b);

  //TESTA U-SUB
  printf("\n");
  bi_printf(a); printf(" - ");
  bi_printf(b); printf(" = ");

  bi_resize(res, MAX(a->limbs, b->limbs)+1);
  bi_usub(res,b,a);
  bi_normalize(res);
  bi_printf(res);

 printf("\n");

    
  bi_clear(a);
  bi_clear(b);
  bi_clear(res);

}





void main(int argc, char **argv) {
  time_t t;
  eget_test();


  srand((unsigned) time(&t));

  if (argc == 2 && strcmp(argv[1], "-h") == 0) {
    printf("%s [-v [-a]]\n", argv[0]);
    printf("Reads test cases from stdin by default.\n");
    printf("-v use verbose output.\n");
    printf("-a run all tests and ignore stdin.\n");
    exit(0);
  } else if (argc > 1) {
    if (strcmp(argv[1], "-v") == 0) {
      verbose = 1;
    } else {
      printf("Expected the -v flag and not %s!\n", argv[1]);
      exit(1);
    }
  }

  if (argc > 2) {

    if (argc == 3 && strcmp(argv[2], "-a") == 0) {
      verbose = 1;

      bi_test_conv();
      bi_test_impexp();

      bi_test_v3("bi_add", bi_add, mpz_add);
      bi_test_v3("bi_sub", bi_sub, mpz_sub);
      bi_test_v3("bi_mul", bi_mul, mpz_mul);

      bi_test_v3("bi_and", bi_and, mpz_and);
      bi_test_v3("bi_or", bi_or, mpz_ior);
      bi_test_v3("bi_xor", bi_xor, mpz_xor);

      bi_test_cmp();

      bi_test_bitsize();
      bi_test_tstbit();
    } else {
      printf("Expected the -a flag and not %s!\n", argv[1]);
      exit(1);
    }
  } else {
    runtests();
  }
}
