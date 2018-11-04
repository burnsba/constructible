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

// For the data structures defined in this project, they
// should be malloc'd and then memset to zero. Once they are
// initialized, the is_init property is set to this value.
// This is a safety check to make sure they are initialized 
// properly before use.
#define IS_INIT 0xAF

// Parameter passed to GMP to set the default precision.
// default: 200
#define PRECISION_BITS 200

// Ok, this number should be the base 10 representation of the number of significant
// digits, which should be ~ ln(2^PRECISION_BITS)/ln(10). Really, you should be able
// to assume this, but it seems it is sometimes higher than what is calculated. I'm
// not sure why. But if this is larger than half the HASH_KEY_LENGTH (below) then
// the second cordinate (y) will suffer from loss of precision. That just means
// the hashing function might determine two different points (different based on the
// the precision bits) but with sufficiently close y values will hash to the same 
// value. The fix is to just define this explicitly.
// default: 60
#define HASH_DECIMAL_DIGITS 60

// hash key should be able to accomodate full 2d coordinate as a string: {x,y}
// Will need to accomodate signx2, decimal pointx2, comma separator, zero termination char.
// The number of digits needed for each coordinate is ~ ln(2^PRECISION_BITS)/ln(10).
// But see the notes for HASH_DECIMAL_DIGITS.
// default: 136
#define HASH_KEY_LENGTH 136

// Number of decimal digits to use when printing output. This is smaller than
// the above to avoid extra clutter.
#define PRINT_DIGITS 10

// Will attempt to construct points only this many times.
#define MAX_ITERATIONS 2

// When iterating over objects, enabling this will describe what is 
// being checked. Output like:
// check {0.0000000000, 0.0000000000} -> {1.0000000000} x {0.0000000000, 1.0000000000} -> {1.0000000000}
#define PRINT_OBJECT_DESCRIPTION_IN_INTERSECTION_CHECK 0

// When iterating over objects, enabling will print how many points
// where found for that intersect. Output like:
// 2 intersections found.
#define PRINT_NUMBER_INTERSECTIONS_FOUND 0

// Enabling this will print a message when adding a point to the hash
// of known points.
#define PRINT_ADDING_COORDS 0

// Enabling this will print out some numbers at the end of each iteration.
#define PRINT_ITERATION_STATS 1

// If more than this many seconds have passed since the last status 
// update, write a status update in the inner p3/p4 loop.
// Set to zero or negative to disable
#define ELAPSED_TIME_UPDATE_SEC 5

// After everything is done, sort the points and write the output to a file.
// This file is truncated and overwritten.
#define WRITE_POINTS_TO_FILE 1
#define OUTPUT_FILENAME "data.out"

// Save output points as they are found instead of waiting until the end.
// This file is truncated and overwritten.
// No problems will occur if this is the same as OUTPUT_FILENAME,
// but this will be overwritten with the sorted version (if used).
// WARNING: this can have severe performance impact.
#define INCREMENTAL_WRITE_POINTS_TO_FILE 0
#define INCREMENTAL_OUTPUT_FILENAME "data_incremental.out"

// Global constant, initialized to the number 0
extern mpf_t g_zero;

// Global constant, initialized to the number 1
extern mpf_t g_one;

// Global constant, initialized to the number 2
extern mpf_t g_two;

// absolute values less than this will be considered zero
extern mpf_t g_epsilon;
#define STR_EPSILON "0.00000000000000000001"

/*
* Set the precision of GMP. By default, this is called with PRECISION_BITS.
* This only affects variables instantiated after this call.
* Initializes some static variables.
*/
void global_init(mp_bitcnt_t);

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

#endif