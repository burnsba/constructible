/*
* Two dimensional point.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#ifndef __POINT_H__
#define __POINT_H__

#include <stdio.h> // recommended to include stdio before gmp
#include <gmp.h>
#include <stdint.h>

#include "global.h"
#include "uthash.h"

typedef struct point {
    // x coordinate.
    mpf_t x;
    
    // y coordinate.
    mpf_t y;
    
    // Will contain the unique representation of the point, up to precision,
    // as a zero terminated string: "-x.xxxxx,-y.yyyyyy"
    char hash_id[HASH_KEY_LENGTH];
    
    // Whether or not this object has been initialized.
    int is_init;
    
    // makes this structure hashable
    UT_hash_handle hh;
} point_t;

/*
* Initializes some static variables.
*/
void global_point_init();

/*
* Frees memory used by static variables.
*/
void global_point_free();

/*
* Allocates memory for a new point.
*
* returns: pointer to new point.
*/
point_t* point_alloc();

/*
* Initializes new point. Must be called before use.
*
* @p: Point to initialize.
*/
void point_init(point_t* p);

/*
* Frees resources used by the point.
*
* @p: Point to free.
*/
void point_free(point_t* p);

/*
* Copies the x,y values from one point to another.
* This updates the point's hash key.
*
* @copy_to: Point that will get new x,y values.
* @copy_from: Point x,y values are copied from.
*/
void point_copy(point_t* copy_to, point_t* copy_from);

/*
* Allocates memory and copies a point there.
*
* @p: Point to copy.
*
* returns: points to new point.
*/
point_t* point_clone(point_t* p);

/*
* Sets a point's x,y values via GMP parameter.
* This updates the point's hash key.
*
* @p: Point that will get new x,y values.
* @x: x value.
* @y: y value.
*/
void point_set(point_t* p, mpf_t x, mpf_t y);

/*
* Sets a point's x,y values via int.
* This updates the point's hash key.
*
* @p: Point that will get new x,y values.
* @x: x value.
* @y: y value.
*/
void point_set_si(point_t* p, intmax_t x, intmax_t y);

/*
* Sets the point's hash key. This is required
* to be called before using the hash key.
*
* @p: Point to update hash key.
*/
void point_ensure_hash(point_t* p);

/*
* Determines the distance between two points.
* The distance is unrounded in that there is no comparison to g_epsilon.
*
* @rop: Calculated distance between the points.
* @p1: First point.
* @p2: Second point.
*/
void point_distance(mpf_t rop, point_t* p1, point_t* p2);

/*
* Checks if two points are sufficiently close together to 
* be considered the same point.
*
* @p1: First point.
* @p2: Second point.
*
* returns: 1 if the distance between the two points is less 
*     than g_epsilon, 0 otherwise.
*/
int point_equals(point_t* p1, point_t* p2);

/*
* Writes the point to stdout in "the usual way."
*
* @p: Point to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void point_printf(point_t* p, size_t n_digits);

/*
* Writes the point to stdout in "the usual way"
* but with a newline appended.
*
* @p: Point to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void point_printfn(point_t* p, size_t n_digits);

/*
* Writes the point to file in "the usual way."
* It is assumed the file is already opened for writing.
*
* @fp: Pointer to file.
* @p: Point to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void point_fprintf(FILE* fp, point_t* p, size_t n_digits);

/*
* Sort method for uthash. Points are compared using global_compare2,
* which will round if the difference is less than g_epsilon.
*
* @a: First point.
* @b: Second point.
*
* returns: if (a->x == b->x) then compare a->y to b->y. Otherwise,
* compare a->x to b->x, where the comparison is: if (a<b) then -1, 
* if (a==b) then 0, if (a>b) then 1.
*/
int point_sort_function(void *a, void *b);

#endif