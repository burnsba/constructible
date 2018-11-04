/*
* Main program.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#include <assert.h>
#include <stdio.h> // recommended to include stdio before gmp
#include <gmp.h>
#include <sys/time.h>
#include <unistd.h>

#include "global.h"
#include "point.h"
#include "line.h"
#include "circle.h"
#include "test.h"
#include "uthash.h"
#include "list.h"

int add_if_new_else_free(point_t**, point_t**);
int add_line_x_line(point_t**, line_t*, line_t*);
int add_circle_x_line(point_t**, circle_t*, line_t*);
int add_circle_x_circle(point_t**, circle_t*, circle_t*);
void incremental_file_write(point_t* p);

int add_line_x_line(point_t** known_points, line_t* line_one, line_t* line_two) {
    point_t* ip1 = NULL;
    int newly_added_points = 0;
    int result = 0;
    
    if (PRINT_OBJECT_DESCRIPTION_IN_INTERSECTION_CHECK) {
        printf("check ");
        line_printf(line_one, PRINT_DIGITS);
        printf(" x ");
        line_printfn(line_two, PRINT_DIGITS);
    }
    
    result = line_intersection_line(line_one, line_two, &ip1);
    
    if (PRINT_NUMBER_INTERSECTIONS_FOUND) {
        printf("%d intersections found.\n", result);
    }
    
    newly_added_points += add_if_new_else_free(known_points, &ip1);
    
    return newly_added_points;
}

int add_circle_x_line(point_t** known_points, circle_t* c1, line_t* line) {
    point_t* ip1 = NULL;
    point_t* ip2 = NULL;
    int newly_added_points = 0;
    int result = 0;
    
    if (PRINT_OBJECT_DESCRIPTION_IN_INTERSECTION_CHECK) {
        printf("check ");
        circle_printf(c1, PRINT_DIGITS);
        printf(" x ");
        line_printfn(line, PRINT_DIGITS);
    }
    
    result = circle_intersection_line(c1, line, &ip1, &ip2);

    if (PRINT_NUMBER_INTERSECTIONS_FOUND) {
        printf("%d intersections found.\n", result);
    }

    newly_added_points += add_if_new_else_free(known_points, &ip1);
    newly_added_points += add_if_new_else_free(known_points, &ip2);
    
    return newly_added_points;
}

int add_circle_x_circle(point_t** known_points, circle_t* c1, circle_t* c2) {
    point_t* ip1 = NULL;
    point_t* ip2 = NULL;
    int newly_added_points = 0;
    int result = 0;
    
    if (PRINT_OBJECT_DESCRIPTION_IN_INTERSECTION_CHECK) {
        printf("check ");
        circle_printf(c1, PRINT_DIGITS);
        printf(" x ");
        circle_printfn(c2, PRINT_DIGITS);
    }
    
    result = circle_intersection_circle(c1, c2, &ip1, &ip2);

    if (PRINT_NUMBER_INTERSECTIONS_FOUND) {
        printf("%d intersections found.\n", result);
    }

    newly_added_points += add_if_new_else_free(known_points, &ip1);
    newly_added_points += add_if_new_else_free(known_points, &ip2);
    
    return newly_added_points;
}

// returns the number of added points
int add_if_new_else_free(point_t** p_point_hash, point_t** p) {
    point_t* s;
    point_t* ip = *p;
    
    if (ip != NULL) {
        point_ensure_hash(ip);
        
        // has intersection point been seen before?
        HASH_FIND_STR(*p_point_hash, ip->hash_id, s);
        if (s == NULL) {
            // not seen, add to list
            
            if (PRINT_ADDING_COORDS) {
                printf("Adding: %s\n", ip->hash_id);
            }
            
            HASH_ADD_STR(*p_point_hash, hash_id, ip);
            
            incremental_file_write(ip);
            
            return 1;
        } else {
            point_free(ip);
            *p = NULL;
        }
    }
    
    return 0;
}

void incremental_file_write(point_t* p) {
    if (INCREMENTAL_WRITE_POINTS_TO_FILE) {
                
        FILE *fp;
        fp = fopen(INCREMENTAL_OUTPUT_FILENAME, "a+");
        
        if (NULL == fp) {
            fprintf(stderr, "Fatal error opening %s for output.\n", INCREMENTAL_OUTPUT_FILENAME);
            exit(1);
        }
        
        point_fprintf(fp, p, HASH_DECIMAL_DIGITS);
        fclose(fp);
    }
}

int main() {
    
    // init
    
    global_init(PRECISION_BITS);
    global_point_init();
    global_line_init();
    global_circle_init();
    
    // verify
    test_run();
    
    // done initializing.
    
    // declare variables to work with
    
    // track current iteration number
    int current_iteration = 0;
    
    // track the results for each iteration
    size_t iteration_count[MAX_ITERATIONS+1];
    
    // reused variable, return value for functions
    int result;
    
    // reused count variable, and counters for the 4 point loops
    size_t count, p1_count, p2_count, p3_count, p4_count;
    
    // count the number of points added each iteration
    size_t newly_added_points = 0;
    
    // The four points used in the four point loops.
    point_t* p1, *p2, *p3, *p4;
    
    // Distances between points.
    mpf_t d1, d2;
    
    // Lines and circles generated from the 4 points.
    line_t* left_line, *right_line;
    circle_t* left_circle1, *left_circle2, *right_circle1, *right_circle2;
    
    // Setup a preliminary list to load initial starting points.
    // Duplicates will be ignored.
    list_t* starting_set = NULL;
    
    // Primary list used during iteration.
    list_t* working_set = NULL;
    
    // Hash of all known points.
    point_t* known_points = NULL;
    
    // node to iterate starting set.
    list_t* n1;
    
    // Iterate the working_set 4 times, one for each point.
    list_t* p1_node, *p2_node, *p3_node, *p4_node;
    
    // Variables for watching elapsed time since last 
    // status update.
    struct timeval start_tv, tv1, tv2;
    double update_elapsed;
    double total_elapsed;
        
    // inititalize objects
    
    memset(iteration_count, 0, (MAX_ITERATIONS+1)*sizeof(size_t));
    
    mpf_init(d1);
    mpf_init(d2);
    
    // truncate incremental output file
    if (INCREMENTAL_WRITE_POINTS_TO_FILE) {
        FILE *file = fopen(INCREMENTAL_OUTPUT_FILENAME, "w");
        fclose(file);
    }
    
    // setup starting conditions
    
    // set signed int point like so:
    //p1 = point_alloc();
    //point_init(p1);
    //point_set_si(p1, 1, 0);
    //list_add(&starting_set, p1, sizeof(list_t));
    
    // gmp str set point:
    //p1 = point_alloc();
    //point_init(p1);
    //mpf_set_str(d1, "-0.5", 10);
    //point_set(p1, d1, g_zero);
    //list_add(&starting_set, p1, sizeof(list_t));
    
    // gmp sqrt/2 point:
    //p1 = point_alloc();
    //point_init(p1);
    //mpf_sqrt_ui(d1, 3); 
    //mpf_div_ui(d2, d1, 2); 
    //point_set(p1, g_zero, d2);
    //list_add(&starting_set, p1, sizeof(list_t));
        
    p1 = point_alloc();
    point_init(p1);
    point_set_si(p1, 0, 0);
    list_add(&starting_set, p1, sizeof(list_t));
        
    p1 = point_alloc();
    point_init(p1);
    point_set_si(p1, 0, 1);
    list_add(&starting_set, p1, sizeof(list_t));
    
    // load initial points into working_set
    
    printf("Starting with the following points: \n");
    
    for (n1 = starting_set; n1 != NULL; n1=n1->next) {
        point_t* s;
        p1 = n1->data;
        HASH_FIND_STR(known_points, p1->hash_id, s);
        if (s == NULL) {
            point_printfn(p1, PRINT_DIGITS);
            HASH_ADD_STR(known_points, hash_id, p1);
            // note: known_points and working_set need different references for point
            p2 = point_clone(p1);
            list_add(&working_set, p2, sizeof(list_t));
            newly_added_points++;
            
            incremental_file_write(p1);
        }
    }
    
    iteration_count[0] = newly_added_points;
    
    printf("\n");
    
    gettimeofday(&start_tv, NULL);
    gettimeofday(&tv1, NULL);
    
    // main loop
    for (current_iteration=1; current_iteration<=MAX_ITERATIONS; current_iteration++) {
        
        newly_added_points = 0;
        
        /*
        * The algorithm for finding constructible points is as follows:
        * 1) Start with a set of points (working_set).
        * 2) Iterate over every possible pair, and
        * 3) construct a line, left circle, and right circle from the pair.
        * 4) Iterate over every possible pair of objects in (3) and,
        * 5) find the intersections.
        *
        * In code below:
        * 2) The double for loop iterates over working_set and sets p1, p2.
        * 3) p1,p2 are used to build left_line, left_circle1, left_circle2.
        * 4) Pairs from working_set is iterated again to give p3,p4.
        *     p3,p4 are used to build right_line, right_circle1, right_circle2.
        * 5) The 9 possible combinations of lines and circles are checked 
        *     for intersecting points.       
        *     A hash of points already known is tracked in known_points.
        * 6) At the end of the iteration:
        * 6.1) working_set is emptied.
        * 6.2) the (unique) points from known_points are moved to working_set.
        * 6.3) Repeat at step (1) until the required number of iterations.
        */
        
        for (p1_node = working_set, p1_count=0; p1_node != NULL; p1_node = p1_node->next, p1_count++) {
            for (p2_node = p1_node->next, p2_count=0; p2_node != NULL; p2_node = p2_node->next, p2_count++) {
                p1 = (point_t*)p1_node->data;
                p2 = (point_t*)p2_node->data;
                
                point_distance(d1, p1, p2);
                
                // skip if points are the same
                if (global_is_zero(d1) == 1) {
                    continue;
                }
                
                left_line = line_alloc();
                left_circle1 = circle_alloc();
                left_circle2 = circle_alloc();
                
                line_init(left_line);
                circle_init(left_circle1);
                circle_init(left_circle2);
                
                line_set(left_line, p1, p2);
                circle_set(left_circle1, p1, d1);
                circle_set(left_circle2, p2, d1);
                
                // Self intersections for left points (three total)
                
                // (x1)
                newly_added_points += add_circle_x_line(&known_points, left_circle1, left_line);
                
                // (x2)
                newly_added_points += add_circle_x_line(&known_points, left_circle2, left_line);
                
                // (x3)
                newly_added_points += add_circle_x_circle(&known_points, left_circle1, left_circle2);
                
                // first pair covered, onto the second pair
                for (p3_node = p1_node, p3_count=0; p3_node != NULL; p3_node = p3_node->next, p3_count++) {
                    for (p4_node = p3_node->next, p4_count=0; p4_node != NULL; p4_node = p4_node->next, p4_count++) {
                        
                        if (p1_node == p3_node && p2_node == p4_node) {
                            continue;
                        }
                        
                        // This measures how much time has elapsed since the last status update.
                        // If enough time has passed, print a new status updated.
                        // (add a delay to test this with: sleep(1))
                        if (ELAPSED_TIME_UPDATE_SEC > 0) {
                            gettimeofday(&tv2, NULL);
                            update_elapsed = (double) (tv2.tv_sec - tv1.tv_sec);
                            if (update_elapsed > (double)(ELAPSED_TIME_UPDATE_SEC)) {
                                total_elapsed = (double)(tv2.tv_sec - start_tv.tv_sec);
                                count = HASH_COUNT(known_points);
                                printf("%llu: p1=%zu, p2=%zu, p3=%zu, p4=%zu, working_set length=%zu, known_points=%zu\n",
                                    (long long unsigned)total_elapsed,
                                    p1_count,
                                    p2_count,
                                    p3_count,
                                    p4_count,
                                    working_set->index,
                                    count
                                    );
                                gettimeofday(&tv1, NULL);
                            }
                        }                 
                        
                        p3 = (point_t*)p3_node->data;
                        p4 = (point_t*)p4_node->data;
                        
                        point_distance(d2, p3, p4);
                        
                        // skip if points are the same
                        if (global_is_zero(d2) == 1) {
                            continue;
                        }
                        
                        right_line = line_alloc();
                        right_circle1 = circle_alloc();
                        right_circle2 = circle_alloc();
                        
                        line_init(right_line);
                        circle_init(right_circle1);
                        circle_init(right_circle2);
                        
                        line_set(right_line, p3, p4);
                        circle_set(right_circle1, p3, d2);
                        circle_set(right_circle2, p4, d2);
                        
                        // All comparisons:
                        // (1) left_line    x right_line, (2) left_line x right_circle1,    (3) left_line x right_circle2
                        // (4) left_circle1 x right_line, (5) left_circle1 x right_circle1, (6) left_circle1 x right_circle2
                        // (7) left_circle2 x right_line, (8) left_circle2 x right_circle1, (9) left_circle2 x right_circle2
                        
                        // sometimes, two of p1,p2,p3,p4 will be the same. Handle that here.
                        if (p1 == p3)
                        {
                            // skip (1) left_line x right_line
                            // skip (5) left_circle1 x right_circle1
                            // (6)
                            newly_added_points += add_circle_x_circle(&known_points, left_circle1, right_circle2);
                        }
                        else if (p2 == p3)
                        {
                            // skip (1) left_line x right_line
                            // skip (6) left_circle1 x right_circle2
                            // (5)
                            newly_added_points += add_circle_x_circle(&known_points, left_circle1, right_circle1);
                        }
                        else
                        {
                            // (1)
                            newly_added_points += add_line_x_line(&known_points, left_line, right_line);
                            
                            // (5)
                            newly_added_points += add_circle_x_circle(&known_points, left_circle1, right_circle1);
                            
                            // (6)
                            newly_added_points += add_circle_x_circle(&known_points, left_circle1, right_circle2);
                        }

                        // always check everything else
                            
                        // (1)
                        // handled above
                            
                        // (2)
                        newly_added_points += add_circle_x_line(&known_points, right_circle1, left_line);
                        
                        // (3)
                        newly_added_points += add_circle_x_line(&known_points, right_circle2, left_line);

                        // (4)
                        newly_added_points += add_circle_x_line(&known_points, left_circle1, right_line);
                        
                        // (5)
                        // handled above
                        
                        // (6)
                        // handled above
                        
                        // (7)
                        newly_added_points += add_circle_x_line(&known_points, left_circle2, right_line);
                        
                        // (8)
                        newly_added_points += add_circle_x_circle(&known_points, left_circle2, right_circle1);
                            
                        // (9)
                        newly_added_points += add_circle_x_circle(&known_points, left_circle2, right_circle2);

                        // And self intersections for right points (three total)
                        
                        // (x1)
                        newly_added_points += add_circle_x_line(&known_points, right_circle1, right_line);
                        
                        // (x2)
                        newly_added_points += add_circle_x_line(&known_points, right_circle2, right_line);
                        
                        // (x3)
                        newly_added_points += add_circle_x_circle(&known_points, right_circle1, right_circle2);
                        
                        // done with right pair
                        
                        line_free(right_line);
                        circle_free(right_circle1);
                        circle_free(right_circle2);
                    }
                }
                
                // done with left pair
                
                line_free(left_line);
                circle_free(left_circle1);
                circle_free(left_circle2);
            }
        }
        
        // done with the current iteration.
        
        if (PRINT_ITERATION_STATS) {
            printf("results for iteration %d\n", current_iteration);
            
            count = working_set->index + 1;
            printf("working_set count: %zu\n", count);
            
            count = HASH_COUNT(known_points);
            printf("known_points count: %zu\n", count);
            
            printf("new points this iteration: %zu\n", newly_added_points);
            printf("\n");
        }
        
        iteration_count[current_iteration] = iteration_count[current_iteration-1] + newly_added_points;
        
        // Empty (and free) current working set.
        // Note: this is why a point cannot reside in both working_set and known_points.
        do {
            if (working_set != NULL) {
                point_free(working_set->data);
            }

            result = list_remove(&working_set);
        } while (1 == result);
        
        // Build new working_set from known_points.
        HASH_ITER(hh, known_points, p1, p2) {
            p3 = point_clone(p1);
            list_add(&working_set, p3, sizeof(list_t));
        }
    }
    
    if (WRITE_POINTS_TO_FILE) {
        // Write a status update, sorting can take a long time.
        printf("Sorting points, about to write to file.\n");
        
        HASH_SORT(known_points, point_sort_function);
        
        FILE *fp;
        fp = fopen(OUTPUT_FILENAME, "w+");
        
        if (NULL == fp) {
            fprintf(stderr, "Fatal error opening %s for output.\n", OUTPUT_FILENAME);
            exit(1);
        }
        
        HASH_ITER(hh, known_points, p1, p2) {
            point_fprintf(fp, p1, HASH_DECIMAL_DIGITS);
        }
        fclose(fp);
    }
    
    // show results
    
    printf("\n");
    
    for (int i=0; i<=MAX_ITERATIONS; i++) {
        printf("%zu", iteration_count[i]);
        if (i + 1 <= MAX_ITERATIONS) {
            printf(",");
        }
    }
    printf("\n");
    
    // done with program, cleanup.
    
    do {
        if (starting_set != NULL) {
            point_free(starting_set->data);
        }

        result = list_remove(&starting_set);
    }while (1 == result);
    
    do {
        if (working_set != NULL) {
            point_free(working_set->data);
        }

        result = list_remove(&working_set);
    }while (1 == result);
    
    HASH_ITER(hh, known_points, p1, p2) {
        HASH_DEL(known_points, p1);
        point_free(p1);
    }
    
    mpf_clear(d1);
    mpf_clear(d2);
    
    global_free();
    global_point_free();
    global_line_free();
    global_circle_free();
    
    printf("success.\n");
    
    return 0;
}