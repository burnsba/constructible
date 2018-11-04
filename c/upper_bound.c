/*
* Program to calculate upper bound for constructible count.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#include <assert.h>
#include <stdio.h> // recommended to include stdio before gmp
#include <stdlib.h>
#include <gmp.h>

/*
* The algorithm for finding constructible points is as follows:
* 1) Start with a set of points.
* 2) Iterate over every possible pair, and
* 3) construct a line, left circle, and right circle from the pair.
* 4) Iterate over every possible pair of objects in (3) and,
* 5) find the intersections.
*
* To find the upper bound for this is straight forward.
* 1) Let n be the number of points.
* 2) The number of possible pairs of n is n*(n-1)/2.
* 3) Multiple (2) by 3, the number of objects. Call this "b":
*     b = 3 * n * (n - 1) / 2
* 4) The number of possible pairs for (3) is b*(b-1)/2.
* 5) The number of possible intersections for each pair is at most 2.
* 6) Therefore, the upper bound is b*(b-1).
*/

/*
* Determines the upper bound for the next iteration of n points.
*/
void calc(mpz_t rop, mpz_t n);

void calc(mpz_t rop, mpz_t n) {
    
    // b = 3 * n * (n - 1) / 2
    // result = b * (b - 1)
    
    // alternatively
    // result = (3 / 4) * n * (n - 1) * (3 * n^2 - 3 * n - 2)
    
    mpz_t t1;
    mpz_t t2;
    mpz_t b;
    mpz_init(t1);
    mpz_init(t2);
    mpz_init(b);
    
    // t1 = n - 1
    mpz_sub_ui(t1, n, 1);
    // t2 = n * (n - 1)
    mpz_mul(t2, n, t1);
    // t1 = 3 * t2
    mpz_mul_ui(t1, t2, 3);
    // b = t1 / 2
    mpz_fdiv_q_ui(b, t1, 2);
    
    // t1 = b - 1
    mpz_sub_ui(t1, b, 1);
    // result = b * (b - 1)
    mpz_mul(rop, b, t1);
    
    mpz_clear(t1);
    mpz_clear(t2);
    mpz_clear(b);
}

int main(int argc, char *argv[]) {
    
    mpz_t t1;
    mpz_t t2;
    mpz_init(t1);
    mpz_init(t2);
    
    int iterations;
    
    if (argc != 3) {
        printf("Requires 2 arguments, start_value and iterations\n");
        exit(0);
    }
    
    mpz_set_str(t1, argv[1], 10);
    iterations = atoi(argv[2]);
    
    for (int i=0; i<iterations; i++) {
        mpz_out_str(stdout, 10, t1);
        if (i < iterations - 1) {
            printf(",");
        }
        calc(t2, t1);
        mpz_set(t1, t2);
    }
    
    printf("\n");
    
    mpz_clear(t1);
    mpz_clear(t2);
    
    return 0;
}