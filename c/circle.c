/*
* Two dimensional circle.
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
#include "circle.h"
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
static mpf_t _td;
static mpf_t _te;
static mpf_t _tf;
static mpf_t _tg;
static mpf_t _th;
static mpf_t _ti;
static mpf_t _tj;
static mpf_t _tk;
static mpf_t _tm;
static mpf_t _tn;
static mpf_t _tp;
static mpf_t _tq;
static mpf_t _tr;
static mpf_t _ts;
static mpf_t _tt;
static mpf_t _tu;
static mpf_t _tv;
static mpf_t _tw;
static mpf_t _tx;
static mpf_t _ty;
static mpf_t _tz;

/*
* Initializes some static variables.
*/
void global_circle_init() {
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
    mpf_init(_td);
    mpf_init(_te);
    mpf_init(_tf);
    mpf_init(_tg);
    mpf_init(_th);
    mpf_init(_ti);
    mpf_init(_tj);
    mpf_init(_tk);
    mpf_init(_tm);
    mpf_init(_tn);
    mpf_init(_tp);
    mpf_init(_tq);
    mpf_init(_tr);
    mpf_init(_ts);
    mpf_init(_tt);
    mpf_init(_tu);
    mpf_init(_tv);
    mpf_init(_tw);
    mpf_init(_tx);
    mpf_init(_ty);
    mpf_init(_tz);
}

/*
* Frees memory used by static variables.
*/
void global_circle_free() {
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
    mpf_clear(_td);
    mpf_clear(_te);
    mpf_clear(_tf);
    mpf_clear(_tg);
    mpf_clear(_th);
    mpf_clear(_ti);
    mpf_clear(_tj);
    mpf_clear(_tk);
    mpf_clear(_tm);
    mpf_clear(_tn);
    mpf_clear(_tp);
    mpf_clear(_tq);
    mpf_clear(_tr);
    mpf_clear(_ts);
    mpf_clear(_tt);
    mpf_clear(_tu);
    mpf_clear(_tv);
    mpf_clear(_tw);
    mpf_clear(_tx);
    mpf_clear(_ty);
    mpf_clear(_tz);
}

/*
* Allocates memory for a new circle.
*
* returns: pointer to new circle.
*/
circle_t* circle_alloc() {
    circle_t* p = malloc(sizeof(circle_t));
    global_exit_if_null(p, "Fatal error calling malloc for circle_t.\n");
    
    memset(p, 0, sizeof(circle_t));
    
    p->origin = point_alloc();
    
    return p;
}

/*
* Initializes new circle. Must be called before use.
*
* @c: Circle to initialize.
*/
void circle_init(circle_t* c) {
    if (c->is_init == IS_INIT)
    {
        return;
    }
    
    point_init(c->origin);
    mpf_init(c->radius);
    c->is_init = IS_INIT;
}

/*
* Frees resources used by the circle.
*
* @c: Circle to free.
*/
void circle_free(circle_t* c) {
    if (c->is_init != IS_INIT)
    {
        return;
    }
    
    point_free(c->origin);
    mpf_clear(c->radius);
    c->is_init = 0;
    free(c);
}

/*
* Sets a circle's origin and radius.
*
* @c: Circle to set.
* @origin: Origin point for the circle.
* @radius: Radius of the circle.
*/
void circle_set(circle_t* c, point_t* origin, mpf_t radius) {
    assert(c->is_init == IS_INIT);
    
    point_set(c->origin, origin->x, origin->y);
    mpf_set(c->radius, radius);
}

/*
* Sets a circle's origin and radius via int.
*
* @c: Circle to set.
* @origin_x: X value for origin point for the circle.
* @origin_y: Y value for origin point for the circle.
* @radius: Radius of the circle.
*/
void circle_set_si(circle_t* c, intmax_t origin_x, intmax_t origin_y, intmax_t radius) {
    assert(c->is_init == IS_INIT);
    
    point_set_si(c->origin, origin_x, origin_y);
    mpf_set_si(c->radius, radius);
}

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
int circle_intersection_line(circle_t* c, line_t* n, point_t** pp1, point_t** pp2) {
    assert(c->is_init == IS_INIT);
    assert(n->is_init == IS_INIT);
    assert(*pp1 == NULL);
    assert(*pp2 == NULL);
    
    // http://paulbourke.net/geometry/circlesphere/source.cpp
    // from http://paulbourke.net/geometry/circlesphere/
    // The original C# this is based on is included at the end of the function.
    
    point_t* p1;
    point_t* p2;
    
    // _t1 = line.P2.X - line.P1.X;
    // _t2 = line.P2.Y - line.P1.Y
    mpf_sub(_t1, n->p2->x, n->p1->x);
    mpf_sub(_t2, n->p2->y, n->p1->y);
    
    // _ta = (_t1)^2 + (_t2)^2;
    mpf_mul(_t3, _t1, _t1);
    mpf_mul(_t4, _t2, _t2);
    mpf_add(_ta, _t3, _t4);
    
    // _tb = Context.TWO * (_t1 * (line.P1.X - this.Origin.X) + _t2 * (line.P1.Y - this.Origin.Y));
    mpf_sub(_t5, n->p1->x, c->origin->x);
    // _tb = Context.TWO * (_t1 * _t5 + _t2 * (line.P1.Y - this.Origin.Y));
    mpf_sub(_t6, n->p1->y, c->origin->y);
    // _tb = Context.TWO * (_t1 * _t5 + _t2 * _t6);
    mpf_mul(_t7, _t1, _t5);
    // _tb = Context.TWO * (_t7 + _t2 * _t6);
    mpf_mul(_t8, _t2, _t6);
    // _tb = Context.TWO * (_t7 + _t8);
    mpf_add(_t9, _t7, _t8);
    // _tb = Context.TWO * _t9;
    mpf_mul(_tb, g_two, _t9);
    
    // _tc = (this.Origin.X)^2 + (this.Origin.Y)^2 + (line.P1.X)^2 + (line.P1.Y)^2 - Context.TWO * (this.Origin.X * line.P1.X + this.Origin.Y * line.P1.Y) - (this.Radius)^2;
    mpf_mul(_td, c->origin->x, c->origin->x);
    mpf_mul(_te, c->origin->y, c->origin->y);
    mpf_mul(_tf, n->p1->x, n->p1->x);
    mpf_mul(_tg, n->p1->y, n->p1->y);
    mpf_mul(_th, c->radius, c->radius);
    // _tc = _td + _te + _tf + _tg - Context.TWO * (this.Origin.X * line.P1.X + this.Origin.Y * line.P1.Y) - _th;
    mpf_mul(_ti, c->origin->x, n->p1->x);
    mpf_mul(_tj, c->origin->y, n->p1->y);
    // _tc = _td + _te + _tf + _tg - Context.TWO * (_ti + _tj) - _th;
    mpf_add(_tk, _ti, _tj);
    // _tc = _td + _te + _tf + _tg - Context.TWO * _tk - _th;
    mpf_mul(_tm, g_two, _tk);
    // _tc = _td + _te + _tf + _tg - _tm - _th;
    mpf_add(_tn, _td, _te);
    // _tc = _tn + _tf + _tg - _tm - _th;
    mpf_add(_tp, _tn, _tf);
    // _tc = _tp + _tg - _tm - _th;
    mpf_add(_tq, _tp, _tg);
    // _tc = _tq - _tm - _th;
    mpf_sub(_tr, _tq, _tm);
    // _tc = _tr - _th;
    mpf_sub(_tc, _tr, _th);
    
    // _tv = _tb * _tb - 4 * _ta * _tc;
    mpf_mul(_ts, _tb, _tb);
    mpf_mul(_tt, _ta, _tc);
    mpf_mul_ui(_tu, _tt, 4);
    mpf_sub(_tv, _ts, _tu);
    
    int cmp = global_compare_zero(_tv);
    
    if (cmp < 0) {
        // no intersection
        return 0;
    } else if (cmp == 0) {
        // one intersection
        
        p1 = point_alloc();
        point_init(p1);
        
        // _ty = -(_tb) / (Context.TWO * _ta);
        mpf_mul(_tw, g_two, _ta);
        mpf_div(_tx, _tb, _tw);
        mpf_neg(_ty, _tx);
        
        // p1 = { line.P1.X + _ty * _t1, line.P1.Y + _ty * _t2 }
        mpf_mul(_tz, _ty, _t1);
        mpf_add(p1->x, n->p1->x, _tz);
        
        // start re-using temp vars
        mpf_mul(_t3, _ty, _t2);
        mpf_add(p1->y, n->p1->y, _t3);
        
        *pp1 = p1;
        
        return 1;
    } else { 
        // cmp > 0
        // two intersections
        p1 = point_alloc();
        p2 = point_alloc();
        point_init(p1);        
        point_init(p2);
        
        // _t4 = g_two * _ta
        mpf_mul(_t4, g_two, _ta);

        // _t7 = (-_tb + Math.Sqrt(_tv)) / _t4;
        mpf_sqrt(_t5, _tv);
        mpf_sub(_t6, _t5, _tb);
        mpf_div(_t7, _t6, _t4);
        
        // p1 { line.P1.X + _t7 * _t1, line.P1.Y + _t7 * _t2 }
        mpf_mul(_t8, _t7, _t1);
        mpf_add(p1->x, n->p1->x, _t8);
        mpf_mul(_t9, _t7, _t2);
        mpf_add(p1->y, n->p1->y, _t9);

        // _tf = (-_tb - Math.Sqrt(_tv)) / _t4;
        // _t5 = Math.Sqrt(_tv) // from above
        mpf_add(_td, _tb, _t5);
        mpf_neg(_te, _td);
        mpf_div(_tf, _te, _t4);
        
        // p2 = { line.P1.X + _tf * (_t1), line.P1.Y + _tf * (_t2) }
        mpf_mul(_tg, _tf, _t1);
        mpf_add(p2->x, n->p1->x, _tg);
        mpf_mul(_th, _tf, _t2);
        mpf_add(p2->y, n->p1->y, _th);
            
        *pp1 = p1;
        *pp2 = p2;
        
        return 2;
    }
    
    /*
    // Here's the C# method this is based on
    public List<Point2> IntersectionInfiniteLine(Line2 line)
    {
        // http://paulbourke.net/geometry/circlesphere/source.cpp
        // from http://paulbourke.net/geometry/circlesphere/

        double a, b, c, i, mu;
        var results = new List<Point2>();

        a = Measure.Square(line.P2.X - line.P1.X) + Measure.Square(line.P2.Y - line.P1.Y);

        b = Context.TWO * ((line.P2.X - line.P1.X) * (line.P1.X - this.Origin.X)
             + (line.P2.Y - line.P1.Y) * (line.P1.Y - this.Origin.Y));

        c = Measure.Square(this.Origin.X) + 
            Measure.Square(this.Origin.Y) +
            Measure.Square(line.P1.X) +
            Measure.Square(line.P1.Y) -
            Context.TWO * (this.Origin.X * line.P1.X + this.Origin.Y * line.P1.Y) - Measure.Square(this.Radius);

        i = b * b - 4 * a * c;
        
        if (Measure.IsZero(i))
        {
            // one intersection
            mu = -b / (Context.TWO * a);

            results.Add(
                new Point2(
                    line.P1.X + mu * (line.P2.X - line.P1.X),
                    line.P1.Y + mu * (line.P2.Y - line.P1.Y)
                    ));

            return results;
        }
        else if (Measure.IsNegative(i))
        {
            // no intersection
            return results;
        }
        else if (Measure.IsPositive(i))
        {
            // two intersections

            mu = (-b + Math.Sqrt(i)) / (Context.TWO * a);
            var p1 = new Point2(
                line.P1.X + mu * (line.P2.X - line.P1.X),
                line.P1.Y + mu * (line.P2.Y - line.P1.Y)
                );

            mu = (-b - Math.Sqrt(i)) / (Context.TWO * a);
            var p2 = new Point2(
                line.P1.X + mu * (line.P2.X - line.P1.X),
                line.P1.Y + mu * (line.P2.Y - line.P1.Y)
                );

            results.Add(p1);

            // last sanity check, make sure the two solutions are distinct
            if (Measure.CheckedDistance(p1, p2) > Context.Epsilon)
            {
                results.Add(p2);
            }

            return results;
        }
        else
        {
            Debug.Assert(false, "Cannot resolve unsolvable equation.");
            return results;
        }
    }
    */
}

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
int circle_intersection_circle(circle_t* c1, circle_t* c2, point_t** pp1, point_t** pp2) {
    assert(c1->is_init == IS_INIT);
    assert(c2->is_init == IS_INIT);
    assert(*pp1 == NULL);
    assert(*pp2 == NULL);
    
    // The original C# this is based on is included at the end of the function.
    
    point_t* p1;
    point_t* p2;
    int delta_sum_cmp;
    int delta_difference_cmp;
    
    // _t1 => radius_sum = c1->radius + c2->radius;
    mpf_add(_t1, c1->radius, c2->radius);
    
    // _t2 => radius_difference = Math.Abs(c1->radius - c2->radius);
    mpf_sub(_t3, c1->radius, c2->radius);
    mpf_abs(_t2, _t3);
    
    // _t3 => dx = c2->origin->x - c1->origin->x;
    mpf_sub(_t3, c2->origin->x, c1->origin->x);
    
    // _t4 => dy = c2->origin->y - c1->origin->y;
    mpf_sub(_t4, c2->origin->y, c1->origin->y);
    
    // _t5 => d = Measure.CheckedDistance(c1->origin, c2->origin);
    point_distance(_t5, c1->origin, c2->origin);
    
    // check if circles have same origin. 
    if (global_is_zero(_t5) == 1) {
        return 0;
    }
    
    // delta_sum_cmp = d ?? radius_sum
    delta_sum_cmp = global_compare2(_t5, _t1);
    
    if (delta_sum_cmp > 0) {
        // d > radius_sum
        // one circle entirely outside the other
        return 0;
    }
    
    // delta_difference_cmp = d ?? radius_difference
    delta_difference_cmp = global_compare2(_t5, _t2);
    
    if (delta_difference_cmp < 0)
    {
        // d < radius_difference
        // one circle entirely inside the other
        return 0;
    }
    
    // If the circles intersect at two points, a line can be drawn
    // between these two points perpendicular to the line between
    // the two origins. Call the intersection of these two lines point p3 = (x3,y3).
    // a = distance from first circle to p3
    
    // a = ((c1->radius * c1->radius) - (c2->radius * c2->radius) + (d * d)) / (Context.TWO * d);
    
    // _t6 = c1->radius * c1->radius
    mpf_mul(_t6, c1->radius, c1->radius);
    // _t7 = c2->radius * c2->radius
    mpf_mul(_t7, c2->radius, c2->radius);
    // _t8 = d * d 
    mpf_mul(_t8, _t5, _t5);
    
    // a = (_t6 - _t7 + _t8) / (Context.TWO * d);
    mpf_sub(_t9, _t6, _t7);
    // a = (_t9 + _t8) / (Context.TWO * d);
    mpf_add(_ta, _t9, _t8);
    // a = _ta / (Context.TWO * d);
    mpf_mul(_t9, g_two, _t5);
    // _tb => a = _ta / _t9;
    mpf_div(_tb, _ta, _t9);
    
    // _te => x3 = c1->origin->x + (dx * a / d);
    // _tf => y3 = c1->origin->y + (dy * a / d);
    mpf_mul(_tc, _t3, _tb);
    mpf_div(_td, _tc, _t5);
    mpf_add(_te, c1->origin->x, _td);
    
    mpf_mul(_tc, _t4, _tb);
    mpf_div(_td, _tc, _t5);
    mpf_add(_tf, c1->origin->y, _td);
    
    if (delta_sum_cmp == 0 || delta_difference_cmp == 0) {
        // Circles are tangent, there is only one intersection point.
        
        p1 = point_alloc();
        point_init(p1);
        
        point_set(p1, _te, _tf);
        
        *pp1 = p1;

        return 1;
    }
    
    // last cases, two intersection points:
    // d > Max(this.Radius, other.Radius)
    // d < Max(this.Radius, other.Radius) && d > radius_difference
    
    // h = distance from p3 to an intersection
    // _ti => h = Math.Sqrt((c1->radius * c1->radius) - (a * a));
    // _tg => a * a
    mpf_mul(_tg, _tb, _tb);
    // found c1->radius squared above: _t6 = c1->radius * c1->radius
    mpf_sub(_th, _t6, _tg);
    // _ti = Math.Sqrt(_th)
    mpf_sqrt(_ti, _th);

    // This is the offset from p3 to the intersection points
    // _tm => rx = -dy * (h / d);
    mpf_neg(_tj, _t4);
    mpf_mul(_tk, _tj, _ti);
    mpf_div(_tm, _tk, _t5);
    // _tn => ry = dx * (h / d);
    mpf_mul(_tj, _t3, _ti);
    mpf_div(_tn, _tj, _t5);

    // p1 = new Point2(x3 + rx, y3 + ry);
    // p2 = new Point2(x3 - rx, y3 - ry);
    
    mpf_add(_tp, _te, _tm);
    mpf_add(_tq, _tf, _tn);
    
    mpf_sub(_tr, _te, _tm);
    mpf_sub(_ts, _tf, _tn);
    
    p1 = point_alloc();
    p2 = point_alloc();
    point_init(p1);
    point_init(p2);
    
    point_set(p1, _tp, _tq);
    point_set(p2, _tr, _ts);
    
    *pp1 = p1;
    *pp2 = p2;
    
    return 2;
    
    // Here's the C# method this is based on
    /*    
    public List<Point2> IntersectionCircle(Circle2 other)
    {
        double a, dx, dy, d, h, rx, ry;
        double x3, y3;

        var results = new List<Point2>();

        double radius_sum = this.Radius + other.Radius;
        double radius_difference = Math.Abs(this.Radius - other.Radius);

        // d is the distance between the two origins, while dx
        // and dy are the horizontal and vertical components
        // of the distance.
        dx = other.Origin.X - this.Origin.X;
        dy = other.Origin.Y - this.Origin.Y;

        d = Measure.CheckedDistance(this.Origin, other.Origin);
        if (Measure.IsZero(d))
        {
            // Circles have same origin
            return results;
        }

        // Because this isn't exact, there should be a small buffer
        // to "definitely" be on one side or the other of these
        // inequalities.

        if (d > radius_sum + Context.Epsilon)
        {
            // one circle entirely outside the other
            return results;
        }

        if (d < radius_difference - Context.Epsilon)
        {
            // one circle entirely inside the other
            return results;
        }

        // If the circles intersect at two points, a line can be drawn
        // between these two points perpendicular to the line between
        // the two origins. Call the intersection of these two lines point p3 = (x3,y3).
        // a = distance from first circle to p3
        a = ((this.Radius * this.Radius) - (other.Radius * other.Radius) + (d * d)) / (Context.TWO * d);

        x3 = this.Origin.X + (dx * a / d);
        y3 = this.Origin.Y + (dy * a / d);

        if (Measure.IsZero(Measure.CheckedDistance(d, radius_sum))
            || Measure.IsZero(Measure.CheckedDistance(d, radius_difference)))
        {
            // Circles are tangent, there is only one intersection point.
            results.Add(new Point2(x3, y3));

            return results;
        }

        // h = distance from p3 to an intersection
        h = Math.Sqrt((this.Radius * this.Radius) - (a * a));

        // This is the offset from p3 to the intersection points
        rx = -dy * (h / d);
        ry = dx * (h / d);

        // last cases, two intersection points:
        // d > Max(this.Radius, other.Radius)
        // d < Max(this.Radius, other.Radius) && d > radius_difference

        var pi1 = new Point2(x3 + rx, y3 + ry);
        var pi2 = new Point2(x3 - rx, y3 - ry);

        results.Add(pi1);

        // last sanity check, make sure the two solutions are distinct
        if (Measure.CheckedDistance(pi1, pi2) > Context.Epsilon)
        {
            results.Add(pi2);
        }

        return results;
    }
    */
}

/*
* Writes the line to stdout in "the usual way."
*
* @n: Line to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void circle_printf(circle_t* c, size_t n_digits) {
    gmp_printf("{%.*Ff, %.*Ff} -> {%.*Ff}", n_digits, c->origin->x, n_digits, c->origin->y, n_digits, c->radius);
}

/*
* Writes the line to stdout in "the usual way"
* but with a newline appended.
*
* @n: Line to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void circle_printfn(circle_t* c, size_t n_digits) {
    gmp_printf("{%.*Ff, %.*Ff} -> {%.*Ff}\n", n_digits, c->origin->x, n_digits, c->origin->y, n_digits, c->radius);
}