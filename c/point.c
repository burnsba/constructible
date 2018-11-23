/*
* Two dimensional point.
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
#include "point.h"

// internal variables use for calculation.
static int _p_init = 0;
static mpf_t _t1;
static mpf_t _t2;
static mpf_t _t3;
static mpf_t _t4;
static mpf_t _t5;

static size_t _str_point_digits;

/*
* Sets the hash key for the point.
*/
static void _set_hash_id(point_t*);

/*
* Initializes some static variables.
*
* @str_point_digits: number of digits to use for each axis when storing point as string.
*/
void global_point_init(size_t str_point_digits) {
    if (_p_init != 0) {
        return;
    }
    
    _p_init = 1;
    
    mpf_init(_t1);
    mpf_init(_t2);
    mpf_init(_t3);
    mpf_init(_t4);
    mpf_init(_t5);
    
    _str_point_digits = str_point_digits;
}

/*
* Frees memory used by static variables.
*/
void global_point_free() {
    if (_p_init != 1) {
        return;
    }
    
    _p_init = 0;
    mpf_clear(_t1);
    mpf_clear(_t2);
    mpf_clear(_t3);
    mpf_clear(_t4);
    mpf_clear(_t5);
}

/*
* Allocates memory for a new point.
*
* returns: pointer to new point.
*/
point_t* point_alloc() {
    point_t* p = malloc(sizeof(point_t));
    global_exit_if_null(p, "Fatal error calling malloc for point_t.\n");
    memset(p, 0, sizeof(point_t));
    
    p->str_x = malloc(sizeof(char)*(_str_point_digits+1));
    global_exit_if_null(p->str_x, "Fatal error calling malloc for point_t->str_x.\n");
    memset(p->str_x, 0, sizeof(char)*(_str_point_digits+1));
    
    p->str_y = malloc(sizeof(char)*(_str_point_digits+1));
    global_exit_if_null(p->str_y, "Fatal error calling malloc for point_t->str_y.\n");
    memset(p->str_y, 0, sizeof(char)*(_str_point_digits+1));
    
    return p;
}

/*
* Initializes new point. Must be called before use.
*
* @p: Point to initialize.
*/
void point_init(point_t* p) {
    if (p->is_init == IS_INIT)
    {
        return;
    }
    
    mpf_init(p->x);
    mpf_init(p->y);
    p->is_init = IS_INIT;
}

/*
* Frees resources used by the point.
*
* @p: Point to free.
*/
void point_free(point_t* p) {
    if(p == NULL)
    {
        return;
    }

    if (p->is_init != IS_INIT)
    {
        return;
    }
    
    mpf_clear(p->x);
    mpf_clear(p->y);
    p->is_init = 0;
    free(p->str_x);
    free(p->str_y);
    free(p);
}

/*
* Copies the x,y values from one point to another.
* This updates the point's hash key.
*
* @copy_to: Point that will get new x,y values.
* @copy_from: Point x,y values are copied from.
*/
void point_copy(point_t* copy_to, point_t* copy_from) {
    assert(copy_to->is_init == IS_INIT);
    assert(copy_from->is_init == IS_INIT);
    
    mpf_set(copy_to->x, copy_from->x);
    mpf_set(copy_to->y, copy_from->y);
    
    // don't bother copying character buffers, will be recalculated below.
    
    _set_hash_id(copy_to);
}


/*
* Allocates memory and copies a point there.
*
* @p: Point to copy.
*
* returns: points to new point.
*/
point_t* point_clone(point_t* p) {
    point_t* pnew = point_alloc();
    point_init(pnew);
    mpf_set(pnew->x, p->x);
    mpf_set(pnew->y, p->y);
    
    // don't bother copying character buffers, will be recalculated below.
    
    _set_hash_id(pnew);
    
    return pnew;
}

/*
* Sets a point's x,y values via GMP parameter.
* This updates the point's hash key.
*
* @p: Point that will get new x,y values.
* @x: x value.
* @y: y value.
*/
void point_set(point_t* p, mpf_t x, mpf_t y) {
    assert(p->is_init == IS_INIT);
    
    mpf_set(p->x, x);
    mpf_set(p->y, y);
    
    _set_hash_id(p);
}

/*
* Sets a point's x,y values via int.
* This updates the point's hash key.
*
* @p: Point that will get new x,y values.
* @x: x value.
* @y: y value.
*/
void point_set_si(point_t* p, intmax_t x, intmax_t y) {
    assert(p->is_init == IS_INIT);
    
    mpf_set_si(p->x, x);
    mpf_set_si(p->y, y);
    
    _set_hash_id(p);
}

/*
* Sets a point's x,y values from string.
* This updates the point's hash key.
*
* @p: Point that will get new x,y values.
* @x: x value.
* @y: y value.
*/
void point_set_str(point_t* p, const char *x, const char *y) {
    assert(p->is_init == IS_INIT);
    
    mpf_set_str(p->x, x, 10);
    mpf_set_str(p->y, y, 10);
    
    _set_hash_id(p);
}

/*
* Sets the point's hash key. This is required
* to be called before using the hash key.
*
* @p: Point to update hash key.
*/
void point_ensure_hash(point_t* p) {
    _set_hash_id(p);
}

/*
* Sets the hash key for the point.
*
* @p: Point to set hash key.
*/
static void _set_hash_id(point_t* p) {
    // Make sure that the value is not negative zero.
    if (global_is_zero(p->x) == 1) {
        mpf_set(p->x, g_zero);
    }
    if (global_is_zero(p->y) == 1) {
        mpf_set(p->y, g_zero);
    }
    
    memset(p->str_x, 0, (_str_point_digits+1));
    memset(p->str_y, 0, (_str_point_digits+1));
    
    gmp_snprintf(p->str_x, _str_point_digits, "%.*Ff", _str_point_digits, p->x);
    gmp_snprintf(p->str_y, _str_point_digits, "%.*Ff", _str_point_digits, p->y);
}

/*
* Determines the distance between two points.
* The distance is unrounded in that there is no comparison to g_epsilon.
*
* @rop: Calculated distance between the points.
* @p1: First point.
* @p2: Second point.
*/
void point_distance(mpf_t rop, point_t* p1, point_t* p2) {
    assert(p1->is_init == IS_INIT);
    assert(p2->is_init == IS_INIT);
    
    if (p1 == p2) {
        mpf_set(rop, g_zero);
        return;
    }
    
    // _t1 = p1.x - p2.x;
    mpf_sub(_t1, p1->x, p2->x);
    // _t2 = _t1 * _t1;
    mpf_mul(_t2, _t1, _t1);
    
    // _t3 = p1.y - p2.y;
    mpf_sub(_t3, p1->y, p2->y);
    // _t4 = _t3 * _t3;
    mpf_mul(_t4, _t3, _t3);
    
    // _t5 = _t2 + _t4;
    mpf_add(_t5, _t2, _t4);
    
    mpf_sqrt(rop, _t5);
}

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
int point_equals(point_t* p1, point_t* p2) {
    assert(p1 != NULL);
    assert(p2 != NULL);

    if (p1 == p2) {
        return 1;
    }

    point_distance(_t1, p1, p2);
    return global_is_zero(_t1);
}

/*
* Writes the point to stdout in "the usual way."
*
* @p: Point to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void point_printf(point_t* p, size_t n_digits) {
    gmp_printf("{%.*Ff, %.*Ff}", n_digits, p->x, n_digits, p->y);
}

/*
* Writes the point to stdout in "the usual way"
* but with a newline appended.
*
* @p: Point to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void point_printfn(point_t* p, size_t n_digits) {
    gmp_printf("{%.*Ff, %.*Ff}\n", n_digits, p->x, n_digits, p->y);
}

/*
* Writes the point to file in "the usual way."
* It is assumed the file is already opened for writing.
*
* @fp: Pointer to file.
* @p: Point to print.
* @n_digits: Precision to be used by gmp_printf.
*/
void point_fprintf(FILE* fp, point_t* p, size_t n_digits) {
    gmp_fprintf(fp, "%.*Ff,%.*Ff\n", n_digits, p->x, n_digits, p->y);
}

/*
* Writes x value of the point to character buffer in "the usual way."
*
* @buffer: Buffer to write to.
* @buffer_len: maximum number of characters to write.
* @p: Point to print.
* @n_digits: Precision to be used by gmp_printf.
*
* returns: The return value is the total number of characters which 
* ought to have been produced, excluding the terminating null. If 
* retval >= size then the actual output has been truncated to the 
* first size-1 characters, and a null appended.
*/
int point_x_snprintf(char* buffer, size_t buffer_len, point_t* p, size_t n_digits) {
    return gmp_snprintf(buffer, buffer_len, "%.*Ff", n_digits, p->x);
}

/*
* Writes y value of the point to character buffer in "the usual way."
*
* @buffer: Buffer to write to.
* @buffer_len: maximum number of characters to write.
* @p: Point to print.
* @n_digits: Precision to be used by gmp_printf.
*
* returns: The return value is the total number of characters which 
* ought to have been produced, excluding the terminating null. If 
* retval >= size then the actual output has been truncated to the 
* first size-1 characters, and a null appended.
*/
int point_y_snprintf(char* buffer, size_t buffer_len, point_t* p, size_t n_digits) {
    return gmp_snprintf(buffer, buffer_len, "%.*Ff", n_digits, p->y);
}

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
int point_sort_function(void *a, void *b) {
    point_t* p1 = (point_t*)a;
    point_t* p2 = (point_t*)b;
    
    int xcmp = global_compare2(p1->x, p2->x);
    if (xcmp == 0) {
        int ycmp = global_compare2(p1->y, p2->y);
        return ycmp;
    }
    return xcmp;
}