/*
* Config values, global constants, common functions.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h> // recommended to include stdio before gmp
#include <gmp.h>
#include <stdarg.h>

// For the data structures defined in this project, they
// should be malloc'd and then memset to zero. Once they are
// initialized, the is_init property is set to this value.
// This is a safety check to make sure they are initialized 
// properly before use.
#define IS_INIT 0xAF

// This is the client id that gets special privileges.
// e.g., only root should copy points from the points_working
// to the points_known table.
#define ROOT_CLIENT_ID 0

// Global constant, initialized to the number 0
extern mpf_t g_zero;

// Global constant, initialized to the number 1
extern mpf_t g_one;

// Global constant, initialized to the number 2
extern mpf_t g_two;

// absolute values less than this will be considered zero
extern mpf_t g_epsilon;

/*
* Set the precision of GMP. By default, this is called with PRECISION_BITS.
* This only affects variables instantiated after this call.
* Initializes some static variables.
*/
void global_init(mp_bitcnt_t, char* str_epsilon);

/*
* Frees any memory used by the global constants defined here.
* Frees memory used by static variables.
*/
void global_free();

/*
* Check if a value is less than g_epsilon.
*
* @f: Value to compare.
*
* returns: 1 if the absolute value is less than g_epsilon, otherwise 0.
*/
int global_is_zero(mpf_t f);

/*
* Compare a value to zero, within range of g_epsilon.
* 
* @f: Value to compare.
*
* returns: 0 if the absolute value is less than g_epsilon,
*     -1 if the value is less than g_zero,
*     or 1 if the value is greater than g_zero.
*/
int global_compare_zero(mpf_t f);

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
int global_compare2(mpf_t f1, mpf_t f2);

/*
* Prints an error message to stderr in red text.
*/
void global_error_printf(char *format, ...);

/*
* Checks if a pointer is null, and if so, prints
* an error message to stderr in red text then exits.
*
* @p: pointer to compare to NULL.
* @error_msg: message to print.
*/
void global_exit_if_null(void* p, char* error_msg);

#endif