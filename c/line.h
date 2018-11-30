/*
* Two dimensional line.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#ifndef __LINE_H__
#define __LINE_H__

#include <stdio.h> // recommended to include stdio before gmp
#include <gmp.h>
#include <stdint.h>

#include "point.h"

// Line is defined by two points.
typedef struct line {
    // First point.
    point_t* p1;
    
    // Second point.
    point_t* p2;
    
    // Whether or not this object has been initialized.
    int is_init;
} line_t;

/*
* Initializes some static variables.
*/
void global_line_init();

/*
* Frees memory used by static variables.
*/
void global_line_free();

/*
* Allocates memory for a new line.
*
* returns: pointer to new line.
*/
line_t* line_alloc();

/*
* Initializes new line. Must be called before use.
*
* @n: Line to initialize.
*/
void line_init(line_t* n);

/*
* Frees resources used by the line.
*
* @n: Line to free.
*/
void line_free(line_t* n);

/*
* Sets a lines's points.
*
* @n: Point that will get new point values.
* @p1: First point.
* @p2: Second point.
*/
void line_set(line_t* n, point_t* p1, point_t* p2);

/*
* Sets a lines's points via int.
*
* @n: Point that will get new point values.
* @p1x: First point x value.
* @p1y: First point y value.
* @p2x: Second point x value.
* @p2y: Second point y value.
*/
void line_set_si(line_t* n, intmax_t p1x, intmax_t p1y, intmax_t p2x, intmax_t p2y);

/*
* Finds the intersection of two infinite lines. If a point is found,
* memory is allocated and the result is stored at the point_t paramter.
* The point_t parameter must be NULL.
* There will be zero or one intersections.
*
* @n1: First line.
* @n2: Second line.
* @p: Double pointer to calculated intersection point.
*
* returns: The number of intersection points found. 
*/
int line_intersection_line(line_t* n1, line_t* n2, point_t** p);

/*
* Writes the line to stdout in "the usual way."
*
* @n: Line to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void line_printf(line_t* n, size_t n_digits);

/*
* Writes the line to stdout in "the usual way"
* but with a newline appended.
*
* @n: Line to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void line_printfn(line_t* n, size_t n_digits);

#endif