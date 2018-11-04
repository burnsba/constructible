/*
* Two dimensional line.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#include <assert.h>
#include <stdio.h> // recommended to include stdio before gmp
#include <gmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "line.h"
#include "point.h"

// internal variables use for calculation.
static int _p_init = 0;
static mpf_t _t1;
static mpf_t _t2;
static mpf_t _t3;
static mpf_t _t4;
static mpf_t _t5;
static mpf_t _t6;
static mpf_t _t7;
static mpf_t _t8;
static mpf_t _t9;
static mpf_t _ta;
static mpf_t _tb;
static mpf_t _tc;

/*
* Initializes some static variables.
*/
void global_line_init() {
    if (_p_init != 0) {
        return;
    }
    
    _p_init = 1;
    
    mpf_init(_t1);
    mpf_init(_t2);
    mpf_init(_t3);
    mpf_init(_t4);
    mpf_init(_t5);
    mpf_init(_t6);
    mpf_init(_t7);
    mpf_init(_t8);
    mpf_init(_t9);
    mpf_init(_ta);
    mpf_init(_tb);
    mpf_init(_tc);
}

/*
* Frees memory used by static variables.
*/
void global_line_free() {
    if (_p_init != 1) {
        return;
    }
    
    _p_init = 0;
    mpf_clear(_t1);
    mpf_clear(_t2);
    mpf_clear(_t3);
    mpf_clear(_t4);
    mpf_clear(_t5);
    mpf_clear(_t6);
    mpf_clear(_t7);
    mpf_clear(_t8);
    mpf_clear(_t9);
    mpf_clear(_ta);
    mpf_clear(_tb);
    mpf_clear(_tc);
}

/*
* Allocates memory for a new line.
*
* returns: pointer to new line.
*/
line_t* line_alloc() {
    line_t* p = malloc(sizeof(line_t));
    if(p == NULL)
    {
        fprintf(stderr, "Fatal error calling malloc for line_t.\n");
        exit(1);
    }
    
    memset(p, 0, sizeof(line_t));
    
    p->p1 = point_alloc();
    p->p2 = point_alloc();
    
    return p;
}

/*
* Initializes new line. Must be called before use.
*
* @n: Line to initialize.
*/
void line_init(line_t* n) {
    if (n->is_init == IS_INIT)
    {
        return;
    }
    
    point_init(n->p1);
    point_init(n->p2);
    n->is_init = IS_INIT;
}

/*
* Frees resources used by the line.
*
* @n: Line to free.
*/
void line_free(line_t* n) {
    if (n->is_init != IS_INIT)
    {
        return;
    }
    
    point_free(n->p1);
    point_free(n->p2);
    n->is_init = 0;
    free(n);
}

/*
* Sets a lines's points.
*
* @n: Point that will get new point values.
* @p1: First point.
* @p2: Second point.
*/
void line_set(line_t* n, point_t* p1, point_t* p2) {
    assert(n->is_init == IS_INIT);
    
    point_set(n->p1, p1->x, p1->y);
    point_set(n->p2, p2->x, p2->y);
}

/*
* Sets a lines's points via int.
*
* @n: Point that will get new point values.
* @p1x: First point x value.
* @p1y: First point y value.
* @p2x: Second point x value.
* @p2y: Second point y value.
*/
void line_set_si(line_t* n, intmax_t x1, intmax_t y1, intmax_t x2, intmax_t y2) {
    assert(n->is_init == IS_INIT);
    
    point_set_si(n->p1, x1, y1);
    point_set_si(n->p2, x2, y2);
}

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
int line_intersection_line(line_t* n1, line_t* n2, point_t** pp) {
    assert(n1->is_init == IS_INIT);
    assert(n2->is_init == IS_INIT);
    assert(*pp == NULL);
    
    // See:
    // http://mathworld.wolfram.com/Line-LineIntersection.html
    
    // The original C# this is based on is included at the end of the function.
    
    point_t* p;
    
    // _t1 = n1->p2->y - n1->p1->y;
    mpf_sub(_t1, n1->p2->y, n1->p1->y);
    // _t2 = n1->p1->x - n1->p2->x;
    mpf_sub(_t2, n1->p1->x, n1->p2->x);
    
    // _t5 = _t1 * n1->p1->x + _t2 * n1->p1->y;
    mpf_mul(_t3, _t1, n1->p1->x);
    mpf_mul(_t4, _t2, n1->p1->y);
    mpf_add(_t5, _t3, _t4);
    
    // _t3 = n2->p2->y - n2->p1->y;
    mpf_sub(_t3, n2->p2->y, n2->p1->y);
    // _t4 = n2->p1->x - n2->p2->x;
    mpf_sub(_t4, n2->p1->x, n2->p2->x);
    
    // _t6 = _t3 * n2->p1->x + _t4 * n2->p1->y;
    mpf_mul(_t7, _t3, n2->p1->x);
    mpf_mul(_t8, _t4, n2->p1->y);
    mpf_add(_t6, _t7, _t8);
    
    // _t9 = _t1 * _t4 - _t3 * _t2;
    mpf_mul(_t7, _t1, _t4);
    mpf_mul(_t8, _t3, _t2);
    mpf_sub(_t9, _t7, _t8);
    
    if (global_is_zero(_t9) == 1) {
        // no intersection
        return 0;
    }
    
    p = point_alloc();
    point_init(p);
    
    // p->x = (_t4 * _t5 - _t2 * _t6) / _t9;
    mpf_mul(_t7, _t4, _t5);
    mpf_mul(_t8, _t2, _t6);
    mpf_sub(_ta, _t7, _t8);
    mpf_div(p->x, _ta, _t9);
    
    // p->y = (_t1 * _t6 - _t3 * _t5) / _t9;
    mpf_mul(_t7, _t1, _t6);
    mpf_mul(_t8, _t3, _t5);
    mpf_sub(_ta, _t7, _t8);
    mpf_div(p->y, _ta, _t9);
    
    *pp = p;
    
    return 1;
    
    // Here's the C# method this is based on
    /*
    public Point2 IntersectionInfinite(Line2 other)
    {
        double ix = double.NaN;
        double iy = double.NaN;

        double a1 = this.P2.Y - this.P1.Y;
        double b1 = this.P1.X - this.P2.X;
        double c1 = a1 * this.P1.X + b1 * this.P1.Y;

        double a2 = other.P2.Y - other.P1.Y;
        double b2 = other.P1.X - other.P2.X;
        double c2 = a2 * other.P1.X + b2 * other.P1.Y;

        double det = a1 * b2 - a2 * b1;

        if (Measure.IsZero(det))
        {
            return null;
        }
        else
        {
            ix = (b2 * c1 - b1 * c2) / det;
            iy = (a1 * c2 - a2 * c1) / det;

            ix = Context.Check(ix);
            iy = Context.Check(iy);
        }

        var p = new Point2(ix, iy);

        if (p.IsFinite())
        {
            return p;
        }

        return null;
    }
    */
}

/*
* Writes the line to stdout in "the usual way."
*
* @n: Line to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void line_printf(line_t* n, size_t n_digits) {
    gmp_printf("{%.*Ff, %.*Ff} <-> {%.*Ff, %.*Ff}", n_digits, n->p1->x, n_digits, n->p1->y, n_digits, n->p2->x, n_digits, n->p2->y);
}

/*
* Writes the line to stdout in "the usual way"
* but with a newline appended.
*
* @n: Line to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void line_printfn(line_t* n, size_t n_digits) {
    gmp_printf("{%.*Ff, %.*Ff} <-> {%.*Ff, %.*Ff}\n", n_digits, n->p1->x, n_digits, n->p1->y, n_digits, n->p2->x, n_digits, n->p2->y);
}