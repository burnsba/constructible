/*
* Config values, global constants, common functions.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#include <stdio.h> // recommended to include stdio before gmp
#include <gmp.h>
#include <assert.h>

#include "global.h"

mpf_t g_one;
mpf_t g_two;
mpf_t g_zero;

mpf_t g_epsilon;

// internal variables use for calculation.
static int _p_init = 0;
static mpf_t _t1;
static mpf_t _t2;

/*
* Set the precision of GMP. By default, this is called with PRECISION_BITS.
* This only affects variables instantiated after this call.
* Initializes some static variables.
*/
void global_init(mp_bitcnt_t precision) {
    if (_p_init != 0) {
        return;
    }

    _p_init = 1;
    
    mpf_set_default_prec(precision);
    
    mpf_init(_t1);
    mpf_init(_t2);
    
    mpf_init_set_ui(g_zero, 0);
    mpf_init_set_ui(g_one, 1);
    mpf_init_set_ui(g_two, 2);
    
    mpf_init_set_str(g_epsilon, STR_EPSILON, 10);
}

/*
* Frees any memory used by the global constants defined here.
* Frees memory used by static variables.
*/
void global_free() {
    if (_p_init != 1) {
        return;
    }
    
    _p_init = 0;
    mpf_clear(_t1);
    mpf_clear(_t2);
    
    mpf_clear(g_zero);
    mpf_clear(g_one);
    mpf_clear(g_two);
    mpf_clear(g_epsilon);
}

/*
* Check if a value is less than g_epsilon.
*
* @f: Value to compare.
*
* returns: 1 if the absolute value is less than g_epsilon, otherwise 0.
*/
int global_is_zero(mpf_t f) {
    mpf_abs(_t1, f);
    
    // Compare op1 and op2. Return a positive value if op1 > op2, zero if op1 = op2, and a negative value if op1 < op2.
    int result = mpf_cmp(_t1, g_epsilon);
    if (result > 0) {
        // absolute value is greater than g_epsilon, so it's not zero.
        return 0;
    }
    
    // absolute value is equal to or less than g_epsilon, so it's "zero"
    return 1;
}

/*
* Compare a value to zero, within range of g_epsilon.
* 
* @f: Value to compare.
*
* returns: 0 if the absolute value is less than g_epsilon,
*     -1 if the value is less than g_zero,
*     or 1 if the value is greater than g_zero.
*/
int global_compare_zero(mpf_t f) {
    mpf_abs(_t1, f);
    int result = mpf_cmp(_t1, g_epsilon);
    if (result < 1) {
        // absolute value is less than or equal to g_epsilon, so it's "zero"
        return 0;
    }
    
    return mpf_sgn(f);
}

/*
* Compare two values, within range of g_epsilon.
*
* @f1: First value to compare.
* @f2: Second value to compare.
*
* returns: 0 if the absolute value of the difference of 
*     f1 and f2 is less than g_epsilon,
*     -1 if f1 is less than f2,
*     or 1 if f1 is greater than f2.
*/
int global_compare2(mpf_t f1, mpf_t f2) {
    mpf_sub(_t1, f1, f2);
    mpf_abs(_t2, _t1);
    int result = mpf_cmp(_t2, g_epsilon);
    if (result < 1) {
        // absolute value of difference is less than or equal to g_epsilon, so it's "zero"
        return 0;
    }
    
    return mpf_sgn(_t1);
}