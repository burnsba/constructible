/*
* Test cases covering use of point, line, circle.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#include <stdio.h> // recommended to include stdio before gmp
#include <gmp.h>
#include <assert.h>

#include "global.h"
#include "point.h"
#include "line.h"
#include "circle.h"

// internal variables use for calculation.
static point_t* _p1;
static point_t* _p2;
static point_t* _pa = NULL; // to be alloc by call
static point_t* _pb = NULL; // to be alloc by call
static line_t* _n1;
static line_t* _n2;
static circle_t* _c1;
static circle_t* _c2;
static int _result;
static int _x1 = 1;
static mpf_t _t1;
static mpf_t _t2;
static mpf_t _t3;
static mpf_t _t4;
static mpf_t _t5;
static mpf_t _t6;

static mpf_t _one_half;
static mpf_t _m_one_half;
static mpf_t _root_two;
static mpf_t _m_root_two;
static mpf_t _root_two_over_two;
static mpf_t _m_root_two_over_two;
static mpf_t _root_three_over_two;
static mpf_t _m_root_three_over_two;

void test_run() {
    
    // General outline of the methods here:
    // - basic point functionality and distance.
    // - line x line intersection
    // - line x circle intersection
    // - circle x circle intersection
    
    mpf_init(_t1);
    mpf_init(_t2);
    mpf_init(_t3);
    mpf_init(_t4);
    mpf_init(_t5);
    mpf_init(_t6);
    
    mpf_init(_one_half);
    mpf_init(_m_one_half);
    mpf_init(_root_two);
    mpf_init(_m_root_two);
    mpf_init(_root_two_over_two);
    mpf_init(_m_root_two_over_two);
    mpf_init(_root_three_over_two);
    mpf_init(_m_root_three_over_two);
    
    mpf_div(_one_half, g_one, g_two);
    mpf_neg(_m_one_half, _one_half);
    mpf_sqrt_ui(_root_two, 2);
    mpf_neg(_m_root_two, _root_two);
    mpf_div_ui(_root_two_over_two, _root_two, 2);
    mpf_neg(_m_root_two_over_two, _root_two_over_two);
    mpf_sqrt_ui(_t1, 3);
    mpf_div_ui(_root_three_over_two, _t1, 2);
    mpf_neg(_m_root_three_over_two, _root_three_over_two);
    
    _p1 = point_alloc();
    _p2 = point_alloc();
    _n1 = line_alloc();
    _n2 = line_alloc();
    _c1 = circle_alloc();
    _c2 = circle_alloc();
    
    point_init(_p1);
    point_init(_p2);
    
    line_init(_n1);
    line_init(_n2);
    
    circle_init(_c1);
    circle_init(_c2);
    
    // point
    
    // uninitialized, but address is the same
    _result = point_equals(_p1, _p1);
    assert(_result == 1); // same
    
    // exact same values
    point_set(_p1, g_one, g_one);
    point_set(_p2, g_one, g_one);
    _result = point_equals(_p1, _p2);
    assert(_result == 1);
    
    // barely same values
    // _p1.x = 1 + (g_epsilon / 10);
    mpf_div_ui(_t1, g_epsilon, 10);
    mpf_add_ui(_p1->x, _t1, _x1);
    _result = point_equals(_p1, _p2);
    assert(_result == 1);
    
    // barely different values
    // _p1.x = 1 + (g_epsilon * 10);
    mpf_mul_ui(_t1, g_epsilon, 10);
    mpf_add_ui(_p1->x, _t1, _x1);
    _result = point_equals(_p1, _p2);
    assert(_result == 0);
    
    // distance({0,0}, {0,1}) == 1
    point_set(_p1, g_zero, g_zero);
    point_set(_p2, g_zero, g_one);
    point_distance(_t1, _p1, _p2);
    assert(global_compare2(_t1, g_one) == 0);
    point_distance(_t1, _p2, _p1);
    assert(global_compare2(_t1, g_one) == 0);
    
    // distance({0,1}, {1,1}) == 1
    point_set(_p1, g_zero, g_one);
    point_set(_p2, g_one, g_one);
    point_distance(_t1, _p1, _p2);
    assert(global_compare2(_t1, g_one) == 0);
    point_distance(_t1, _p2, _p1);
    assert(global_compare2(_t1, g_one) == 0);
    
    // distance({1,1}, {1,0}) == 1
    point_set(_p1, g_one, g_one);
    point_set(_p2, g_one, g_zero);
    point_distance(_t1, _p1, _p2);
    assert(global_compare2(_t1, g_one) == 0);
    point_distance(_t1, _p2, _p1);
    assert(global_compare2(_t1, g_one) == 0);
    
    // distance({0,0}, {1,0}) == 1
    point_set(_p1, g_zero, g_zero);
    point_set(_p2, g_one, g_zero);
    point_distance(_t1, _p1, _p2);
    assert(global_compare2(_t1, g_one) == 0);
    point_distance(_t1, _p2, _p1);
    assert(global_compare2(_t1, g_one) == 0);
    
    // line x line
    // note: calculate intersection twice, but with parameter order swapped
    
    // parallel lines: y = x and y = x - 1
    line_set_si(_n1, 0, 0, 1, 1);
    line_set_si(_n2, 0, -1, 1, 0);
    _result = line_intersection_line(_n1, _n2, &_pa);
    assert(_result == 0);
    assert(_pa == NULL);
    _result = line_intersection_line(_n2, _n1, &_pa);
    assert(_result == 0);
    assert(_pa == NULL);
    
    // parallel lines: y = (2/3)x and y = (2/3)x + 7
    line_set_si(_n1, 0, 0, 3, 2);
    line_set_si(_n2, 0, 7, 3, 9);
    _result = line_intersection_line(_n1, _n2, &_pa);
    assert(_result == 0);
    assert(_pa == NULL);
    _result = line_intersection_line(_n2, _n1, &_pa);
    assert(_result == 0);
    assert(_pa == NULL);
    
    // y = x and y = -x + 1 => {5,5}
    line_set_si(_n1, 0, 0, 1, 1);
    line_set_si(_n2, 0, 10, 10, 0);
    point_set_si(_p1, 5, 5);
    _result = line_intersection_line(_n1, _n2, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    _result = line_intersection_line(_n2, _n1, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    
    // y = 1 and x = 0 => {0, 1}
    line_set_si(_n1, 0, 10, 0, 0);
    line_set_si(_n2, 0, 1, 1, 1);
    point_set_si(_p1, 0, 1);
    _result = line_intersection_line(_n1, _n2, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    _result = line_intersection_line(_n2, _n1, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    
    // y = 2x + 2 and y = -2x -2 => {-1, 0}
    line_set_si(_n1, 0, 2, 1, 4);
    line_set_si(_n2, 0, -2, 1, -4);
    point_set_si(_p1, -1, 0);
    _result = line_intersection_line(_n1, _n2, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    _result = line_intersection_line(_n2, _n1, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    
    // y = 0.01x + 5 and y = 0.0001x + 15 => {(double)100000 / (double)99, (double)1495 / (double)99)}
    line_set_si(_n1, 0, 5, 10000, 105);
    line_set_si(_n2, 0, 15, 10000, 16);
    mpf_set_si(_t1, 100000);
    mpf_set_si(_t2, 99);
    mpf_set_si(_t3, 1495);
    mpf_set_si(_t4, 99);
    mpf_div(_t5, _t1, _t2);
    mpf_div(_t6, _t3, _t4);
    point_set(_p1, _t5, _t6);
    _result = line_intersection_line(_n1, _n2, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    _result = line_intersection_line(_n2, _n1, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    
    // y = 5 and y = 0.0001x + 15 => {-100000, 5}
    line_set_si(_n1, 0, 5, 10, 5);
    line_set_si(_n2, 0, 15, 10000, 16);
    point_set_si(_p1, -100000, 5);
    _result = line_intersection_line(_n1, _n2, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    _result = line_intersection_line(_n2, _n1, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    
    // x = 5 and y = 0.0001x + 15 => {5, 15.0005}
    line_set_si(_n1, 5, 10, 5, 0);
    line_set_si(_n2, 0, 15, 10000, 16);
    mpf_set_si(_t1, 5);
    mpf_set_str(_t2, "15.0005", 10);
    point_set(_p1, _t1, _t2);
    _result = line_intersection_line(_n1, _n2, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    _result = line_intersection_line(_n2, _n1, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    
    // y = (2/3)x + 17/3 and y = (3/2)x => {6.8, 10.2}
    line_set_si(_n1, 5, 9, 8, 11);
    line_set_si(_n2, 2, 3, 4, 6);
    mpf_set_str(_t1, "6.8", 10);
    mpf_set_str(_t2, "10.2", 10);
    point_set(_p1, _t1, _t2);
    _result = line_intersection_line(_n1, _n2, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    _result = line_intersection_line(_n2, _n1, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;

    // same origin
    line_set_si(_n1, 0, 0, 1, 1);
    line_set_si(_n2, 0, 0, 1, 10);
    point_set_si(_p1, 0, 0);
    _result = line_intersection_line(_n1, _n2, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    _result = line_intersection_line(_n2, _n1, &_pa);
    assert(_pa != NULL);
    assert(_result == 1);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    _pa = NULL;
    
    // circle x line
    
    // no intersection
    circle_set_si(_c1, 0, 5, 1);
    line_set_si(_n1, 0, 0, 1, 1);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_line(_c1, _n1, &_pa, &_pb);
    assert(_pa == NULL);
    assert(_pb == NULL);
    assert(_result == 0);
    
    // circle at origin, horizontal line tangent above => {0,1}
    circle_set_si(_c1, 0, 0, 1);
    line_set_si(_n1, -1, 1, 1, 1);
    point_set_si(_p1, 0, 1);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_line(_c1, _n1, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // circle at origin, vertical line tangent on right => {1, 0}
    circle_set_si(_c1, 0, 0, 1);
    line_set_si(_n1, 1, 1, 1, -1);
    point_set_si(_p1, 1, 0);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_line(_c1, _n1, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // circle at origin, horizontal line tangent below => {0, -1}
    circle_set_si(_c1, 0, 0, 1);
    line_set_si(_n1, -1, -1, 1, -1);
    point_set_si(_p1, 0, -1);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_line(_c1, _n1, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // circle at origin, vertical line tangent on left => {-1, 0}
    circle_set_si(_c1, 0, 0, 1);
    line_set_si(_n1, -1, 1, -1, -1);
    point_set_si(_p1, -1, 0);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_line(_c1, _n1, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // tanget at root two over two #1
    circle_set_si(_c1, 0, 0, 1);
    point_set(_n1->p1, g_zero, _root_two);
    point_set(_n1->p2, _root_two, g_zero);
    point_set(_p1, _root_two_over_two, _root_two_over_two);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_line(_c1, _n1, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    
    // tanget at root two over two #2
    circle_set_si(_c1, 0, 0, 1);
    point_set(_n1->p1, g_zero, _m_root_two);
    point_set(_n1->p2, _root_two, g_zero);
    point_set(_p1, _root_two_over_two, _m_root_two_over_two);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_line(_c1, _n1, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    
    // tanget at root two over two #3
    circle_set_si(_c1, 0, 0, 1);
    point_set(_n1->p1, _m_root_two, g_zero);
    point_set(_n1->p2, g_zero, _root_two);
    point_set(_p1, _m_root_two_over_two, _root_two_over_two);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_line(_c1, _n1, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    
    // tanget at root two over two #4
    circle_set_si(_c1, 0, 0, 1);
    point_set(_n1->p1, _m_root_two, g_zero);
    point_set(_n1->p2, g_zero, _m_root_two);
    point_set(_p1, _m_root_two_over_two, _m_root_two_over_two);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_line(_c1, _n1, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    
    // circle at origin and y = x => { Math.Sqrt(2) / 2, Math.Sqrt(2) / 2} and { -Math.Sqrt(2) / 2, -Math.Sqrt(2) / 2}
    circle_set_si(_c1, 0, 0, 1);
    line_set_si(_n1, 0, 0, 1, 1);
    point_set(_p1, _root_two_over_two, _root_two_over_two);
    point_set(_p2, _m_root_two_over_two, _m_root_two_over_two);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_line(_c1, _n1, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // circle at origin and vertical line through origin
    circle_set_si(_c1, 0, 0, 1);
    line_set_si(_n1, 0, 0, 0, 10);
    point_set_si(_p1, 0, 1);
    point_set_si(_p2, 0, -1);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_line(_c1, _n1, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // circle at origin and horizontal line through origin
    circle_set_si(_c1, 0, 0, 1);
    line_set_si(_n1, 0, 0, 10, 0);
    point_set_si(_p1, 1, 0);
    point_set_si(_p2, -1, 0);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_line(_c1, _n1, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // circle x circle
    // note: calculate intersection twice, but with parameter order swapped
    
    // no intersection, one outside the other
    circle_set_si(_c1, 0, 0, 1);
    circle_set_si(_c2, 9, 9, 1);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 0);
    assert(_pa == NULL);
    assert(_pb == NULL);
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 0);
    assert(_pa == NULL);
    assert(_pb == NULL);
    
    // no intersection, one inside the other
    circle_set_si(_c1, 0, 0, 10);
    circle_set_si(_c2, 2, 2, 1);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 0);
    assert(_pa == NULL);
    assert(_pb == NULL);
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 0);
    assert(_pa == NULL);
    assert(_pb == NULL);
    
    // one intersection => {0,1}
    circle_set_si(_c1, 0, 0, 1);
    circle_set_si(_c2, 0, 2, 1);
    point_set_si(_p1, 0, 1);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    
    // one intersection => {1,0}
    circle_set_si(_c1, 0, 0, 1);
    circle_set_si(_c2, 2, 0, 1);
    point_set_si(_p1, 1, 0);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    
    // one intersection => {0, -1}
    circle_set_si(_c1, 0, 0, 1);
    circle_set_si(_c2, 0, -2, 1);
    point_set_si(_p1, 0, -1);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    
    // one intersection => {-1,0}
    circle_set_si(_c1, 0, 0, 1);
    circle_set_si(_c2, -2, 0, 1);
    point_set_si(_p1, -1, 0);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    
    // one intersection => {10,0}
    circle_set_si(_c1, 0, 0, 10);
    circle_set_si(_c2, 11, 0, 1);
    point_set_si(_p1, 10, 0);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 1);
    assert(_pa != NULL);
    assert(_pb == NULL);
    assert(point_equals(_pa, _p1));
    point_free(_pa);
    
    // two intersections => {Math.Sqrt(3) / 2, 1/2} and {-Math.Sqrt(3) / 2, 1/2}
    circle_set_si(_c1, 0, 0, 1);
    circle_set_si(_c2, 0, 1, 1);
    point_set(_p1, _root_three_over_two, _one_half);
    point_set(_p2, _m_root_three_over_two, _one_half);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // two intersections => {Math.Sqrt(3) / 2, -1/2} and {-Math.Sqrt(3) / 2, -1/2}
    circle_set_si(_c1, 0, 0, 1);
    circle_set_si(_c2, 0, -1, 1);
    point_set(_p1, _root_three_over_two, _m_one_half);
    point_set(_p2, _m_root_three_over_two, _m_one_half);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // two intersections => {1/2, Math.Sqrt(3) / 2} and {1/2, -Math.Sqrt(3) / 2}
    circle_set_si(_c1, 0, 0, 1);
    circle_set_si(_c2, 1, 0, 1);
    point_set(_p1, _one_half, _root_three_over_two);
    point_set(_p2, _one_half, _m_root_three_over_two);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // two intersections => {-1/2, Math.Sqrt(3) / 2} and {-1/2, -Math.Sqrt(3) / 2}
    circle_set_si(_c1, 0, 0, 1);
    circle_set_si(_c2, -1, 0, 1);
    point_set(_p1, _m_one_half, _root_three_over_two);
    point_set(_p2, _m_one_half, _m_root_three_over_two);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    
    mpf_add_ui(_t1, _root_three_over_two, 3);
    mpf_add_ui(_t2, _one_half, 3);
    mpf_add_ui(_t3, _m_root_three_over_two, 3);
    mpf_add_ui(_t4, _m_one_half, 3);
    
    // two intersections offset from origin => {3 + Math.Sqrt(3) / 2, 3 + 1/2} and {3-Math.Sqrt(3) / 2, 3+  1/2}
    circle_set_si(_c1, 3, 3, 1);
    circle_set_si(_c2, 3, 4, 1);
    point_set(_p1, _t1, _t2);
    point_set(_p2, _t3, _t2);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // two intersections => {3+Math.Sqrt(3) / 2, 3-1/2} and {3-Math.Sqrt(3) / 2, 3-1/2}
    circle_set_si(_c1, 3, 3, 1);
    circle_set_si(_c2, 3, 2, 1);
    point_set(_p1, _t1, _t4);
    point_set(_p2, _t3, _t4);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // two intersections => {3+1/2, 3+Math.Sqrt(3) / 2} and {3+1/2, 3-Math.Sqrt(3) / 2}
    circle_set_si(_c1, 3, 3, 1);
    circle_set_si(_c2, 4, 3, 1);
    point_set(_p1, _t2, _t1);
    point_set(_p2, _t2, _t3);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // two intersections => {3-1/2, 3+Math.Sqrt(3) / 2} and {3-1/2, 3-Math.Sqrt(3) / 2}
    circle_set_si(_c1, 3, 3, 1);
    circle_set_si(_c2, 2, 3, 1);
    point_set(_p1, _t4, _t1);
    point_set(_p2, _t4, _t3);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c1, _c2, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    _pa = NULL;
    _pb = NULL;
    _result = circle_intersection_circle(_c2, _c1, &_pa, &_pb);
    assert(_result == 2);
    assert(_pa != NULL);
    assert(_pb != NULL);
    assert(point_equals(_pa, _p1) == 1 || point_equals(_pb, _p1) == 1);
    assert(point_equals(_pa, _p2) == 1 || point_equals(_pb, _p2) == 1);
    point_free(_pa);
    point_free(_pb);
    
    // done
    
    point_free(_p1);
    point_free(_p2);
    
    line_free(_n1);
    line_free(_n2);
    
    circle_free(_c1);
    circle_free(_c2);
    
    mpf_clear(_t1);
    mpf_clear(_t2);
    mpf_clear(_t3);
    mpf_clear(_t4);
    mpf_clear(_t5);
    mpf_clear(_t6);
    mpf_clear(_one_half);
    mpf_clear(_m_one_half);
    mpf_clear(_root_two);
    mpf_clear(_m_root_two);
    mpf_clear(_root_two_over_two);
    mpf_clear(_m_root_two_over_two);
    mpf_clear(_root_three_over_two);
    mpf_clear(_m_root_three_over_two);
    
    printf("All tests passed.\n");    
}