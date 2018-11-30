/*
* Two dimensional circle.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include <stdio.h> // recommended to include stdio before gmp
#include <gmp.h>
#include <stdint.h>

#include "point.h"
#include "line.h"
#include "circle.h"

// Circle is defined by the origin point, and a radius.
typedef struct circle {
    // Origin of the circle.
    point_t* origin;
    
    // Radius of the circle.
    mpf_t radius;
    
    // Whether or not this object has been initialized.
    int is_init;
} circle_t;

/*
* Initializes some static variables.
*/
void global_circle_init();

/*
* Frees memory used by static variables.
*/
void global_circle_free();

/*
* Allocates memory for a new circle.
*
* returns: pointer to new circle.
*/
circle_t* circle_alloc();

/*
* Initializes new circle. Must be called before use.
*
* @c: Circle to initialize.
*/
void circle_init(circle_t* c);

/*
* Frees resources used by the circle.
*
* @c: Circle to free.
*/
void circle_free(circle_t* c);

/*
* Sets a circle's origin and radius.
*
* @c: Circle to set.
* @origin: Origin point for the circle.
* @radius: Radius of the circle.
*/
void circle_set(circle_t* c, point_t* origin, mpf_t radius);

/*
* Sets a circle's origin and radius via int.
*
* @c: Circle to set.
* @origin_x: X value for origin point for the circle.
* @origin_y: Y value for origin point for the circle.
* @radius: Radius of the circle.
*/
void circle_set_si(circle_t* c, intmax_t origin_x, intmax_t origin_y, intmax_t radius);

/*
* Finds the intersection of a circle and a line.
* The point_t parameters must be NULL.
* If an intersection is found, memory is allocated and the result
* is stored in the point_t paramter.
* There will be zero, one, or two intersections.
*
* @c: Circle.
* @n: Line.
* @pp1: Double pointer to first calculated intersection point.
* @pp2: Double pointer to second calculated intersection point.
*
* returns: The number of intersection points found.
*/
int circle_intersection_line(circle_t* c, line_t* n, point_t** pp1, point_t** pp2);

/*
* Finds the intersection of a circle and a circle.
* The point_t parameters must be NULL.
* If an intersection is found, memory is allocated and the result
* is stored in the point_t paramter.
* There will be zero, one, or two intersections.
*
* @c1: First circle.
* @c2: Second circle.
* @pp1: Double pointer to first calculated intersection point.
* @pp2: Double pointer to second calculated intersection point.
*
* returns: the number of intersection points found.
*/
int circle_intersection_circle(circle_t* c1, circle_t* c2, point_t** pp1, point_t** pp2);

/*
* Writes the line to stdout in "the usual way."
*
* @n: Line to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void circle_printf(circle_t* c, size_t n_digits);

/*
* Writes the line to stdout in "the usual way"
* but with a newline appended.
*
* @n: Line to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void circle_printfn(circle_t* c, size_t n_digits);

#endif