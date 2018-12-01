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
#include <string.h>
#include <unistd.h>
#include <mysql.h>
#include <stdlib.h>

#include "app_config.h"
#include "mysql_common.h"
#include "datamodel.h"
#include "global.h"
#include "point.h"
#include "line.h"
#include "circle.h"
#include "test.h"
#include "list.h"
#include "ini.h"

app_config_t* _app_config;

// track current iteration number
uint8_t _current_iteration = 0;
int8_t _v = 0;

int add_to_known_and_free(db_context_t* context, point_t** p);
int add_line_x_line(db_context_t* context, line_t*, line_t*);
int add_circle_x_line(db_context_t* context, circle_t*, line_t*);
int add_circle_x_circle(db_context_t* context, circle_t*, circle_t*);

int add_line_x_line(db_context_t* context, line_t* line_one, line_t* line_two) {
    point_t* ip1 = NULL;
    int newly_added_points = 0;
    int result = 0;
    
    if (_app_config->print_object_description_in_intersection_check) {
        printf("check ");
        line_printf(line_one, _app_config->print_digits);
        printf(" x ");
        line_printfn(line_two, _app_config->print_digits);
    }
    
    result = line_intersection_line(line_one, line_two, &ip1);
    
    if (_app_config->print_number_intersections_found) {
        printf("%d intersections found.\n", result);
    }
    
    newly_added_points += add_to_known_and_free(context, &ip1);
    
    return newly_added_points;
}

int add_circle_x_line(db_context_t* context, circle_t* c1, line_t* line) {
    point_t* ip1 = NULL;
    point_t* ip2 = NULL;
    int newly_added_points = 0;
    int result = 0;
    
    if (_app_config->print_object_description_in_intersection_check) {
        printf("check ");
        circle_printf(c1, _app_config->print_digits);
        printf(" x ");
        line_printfn(line, _app_config->print_digits);
    }
    
    result = circle_intersection_line(c1, line, &ip1, &ip2);

    if (_app_config->print_number_intersections_found) {
        printf("%d intersections found.\n", result);
    }

    newly_added_points += add_to_known_and_free(context, &ip1);
    newly_added_points += add_to_known_and_free(context, &ip2);
    
    return newly_added_points;
}

int add_circle_x_circle(db_context_t* context, circle_t* c1, circle_t* c2) {
    point_t* ip1 = NULL;
    point_t* ip2 = NULL;
    int newly_added_points = 0;
    int result = 0;
    
    if (_app_config->print_object_description_in_intersection_check) {
        printf("check ");
        circle_printf(c1, _app_config->print_digits);
        printf(" x ");
        circle_printfn(c2, _app_config->print_digits);
    }
    
    result = circle_intersection_circle(c1, c2, &ip1, &ip2);

    if (_app_config->print_number_intersections_found) {
        printf("%d intersections found.\n", result);
    }

    newly_added_points += add_to_known_and_free(context, &ip1);
    newly_added_points += add_to_known_and_free(context, &ip2);
    
    return newly_added_points;
}

// returns the number of added points
int add_to_known_and_free(db_context_t* context, point_t** p) {
    point_t* ip = *p;
    int result = 0;
    if (ip != NULL) {
        result = db_insert_known_set(context, ip);
        point_free(ip);
        *p = NULL;
    }
    
    return result;
}

void load_starting_points(single_linked_list_t** p_starting_set, char* filename, size_t line_buffer_size) {
    
    size_t half_buffer_size = line_buffer_size / 2;
    ssize_t read_len;
    size_t buffer_len;
    
    char* line_buffer = malloc(sizeof(char) * line_buffer_size);
    global_exit_if_null(line_buffer, "Fatal error calling malloc for line_buffer.\n");
    char* xbuff = malloc(sizeof(char) * half_buffer_size);
    global_exit_if_null(xbuff, "Fatal error calling malloc for xbuff.\n");
    char* ybuff = malloc(sizeof(char) * half_buffer_size);
    global_exit_if_null(ybuff, "Fatal error calling malloc for ybuff.\n");
    
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        global_error_printf("Could not open '%s' for reading\n", filename);
        exit(1);
    }
    
    while ((read_len = getline(&line_buffer, &buffer_len, fp)) != -1) {
        if (line_buffer[0] == ';') {
            continue;
        }
        memset(xbuff, 0, half_buffer_size);
        memset(ybuff, 0, half_buffer_size);
        
        char* comma = strchr(line_buffer, (int)',');
        if (comma == NULL) {
            global_error_printf("Error parsing line: %s\n", line_buffer);
            continue;
        }
        
        if (comma < line_buffer) {
            global_error_printf("Invalid pointer arithmetic\n");
            exit(1);
        }
        
        if (read_len < (comma - line_buffer)) {
            global_error_printf("Invalid pointer arithmetic/line read\n");
            exit(1);
        }
        
        strncpy(xbuff, line_buffer, comma - line_buffer);
        strncpy(ybuff, comma + 1, read_len - (comma - line_buffer));
        
        //printf("read: %s\n", line_buffer);
        //printf("x: %s\n", xbuff);
        //printf("y: %s\n", ybuff);
        //printf("\n");
        
        point_t* p1 = point_alloc();
        point_init(p1);
        point_set_str(p1, xbuff, ybuff);
        single_linked_list_add(p_starting_set, p1, sizeof(single_linked_list_t));
    }
    
    fclose(fp);
    free(xbuff);
    free(ybuff);
    free(line_buffer);
}

int main() {
    
    // read ini
    _app_config = app_config_from_ini("config.ini");
    printf("read config.ini\n");
    app_config_printf(_app_config);
    
    // init
    
    global_init(_app_config->gmp_precision_bits, _app_config->str_init_epsilon);
    global_point_init(_app_config->str_point_digits);
    global_line_init();
    global_circle_init();
    global_datamodel_init();
    
    // verify
    test_run();
    
    // done initializing.
    
    // declare variables to work with
    
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
    single_linked_list_t* starting_set = NULL;
    
    // Primary list used during iteration.
    single_linked_list_t* working_set = NULL;
    
    // node to iterate starting set.
    single_linked_list_t* n1;
    
    // Iterate the working_set 4 times, one for each point.
    single_linked_list_t* p1_node, *p2_node, *p3_node, *p4_node;
    
    // Variables for watching elapsed time since last 
    // status update.
    struct timeval start_tv, tv2;
    double next_status_update_time;
    double next_checkpoint_time;
    double total_elapsed;
    
    size_t loop4_count = 0;
    
    // Current assigned work.
    run_status_t* current_job = NULL;
    
    //      
    mpf_init(d1);
    mpf_init(d2);
    
    // database connection; connect or exit.
    db_context_connect(_app_config->context);
    
    // make commit explicit. This will save on disk i/o,
    // which should make a big difference in throughput, at the
    // risk of losing information (power failure, etc).
    // And of course, now need to explicitly commit changes.
    //mysql_autocommit(_app_config->context->connection->con, 0);
    
    // Check to see if there are any points to work with.
    count = mysql_get_table_count(_app_config->context->connection, _app_config->context->db_table_name_working);
    
    // (set the start time once, in case client quits early).
    gettimeofday(&start_tv, NULL);
    
    if (count == 0) {
        // regular client can't do anything about no points, so quit.
        if (_app_config->client_id != ROOT_CLIENT_ID) {
            printf("Couldn't find work to start. Exiting.\n");
            goto EXIT_LOOP;
        }
        
        // else, this is root, do initial seed.
        // load starting points
        printf("Loading starting points from file.\n");
        load_starting_points(&starting_set,
            _app_config->starting_points_file,
            _app_config->starting_points_file_line_buffer);

        newly_added_points = 0;
        for (n1 = starting_set; n1 != NULL; n1=n1->next) {
            p1 = n1->data;
            newly_added_points += db_insert_known_set(_app_config->context, p1);
        }
        
        if (newly_added_points == 0) {
            printf("Couldn't find starting points to load. Exiting.\n");
            goto EXIT_LOOP;
        }
        
        printf("\n");
    }
    
    // Set time values for status updates.
    gettimeofday(&start_tv, NULL);
    
    next_status_update_time = (double)(start_tv.tv_sec) + (double)(_app_config->update_interval_sec);
    next_checkpoint_time = (double)(start_tv.tv_sec) + (double)(_app_config->checkpoint_interval_sec);
    
    // Ready to start. On to main loop.
    // Book keeping in outer main loop.
    while (1) {
        current_job = db_checkout_work(
            _app_config->context, 
            _app_config->batch_id, 
            _app_config->client_id);
            
        // couldn't checkout anything.
        if (current_job == NULL) {
            // Regular client can't do anything
            if (_app_config->client_id != ROOT_CLIENT_ID) {
                printf("Client found no available work. batch_id=%d\n",
                    _app_config->batch_id);
                break;
            }
            
            root_batch_status_t root_status;
            db_get_root_batch_status(_app_config->context, _app_config->batch_id, &root_status);
            
            // Wait for everyone to finish before advancing iteration.
            if (root_status.is_currently_running == 1 || root_status.any_incomplete == 1) {
                #warning check for hung tasks.
                
                sleep(5);
                continue;
            }
            
            _current_iteration = root_status.last_complete_iteration + 1;
            
            if (_current_iteration > _app_config->max_iterations) {
                printf("All available work is complete. last_complete_iteration=%d, batch_id=%d\n",
                    _current_iteration,
                    _app_config->batch_id);
                break;
            }
            
            printf("Promoting known points.\n");
            db_copy_known_to_working(_app_config->context, _current_iteration);
            
            printf("Creating new tasks.\n");
            db_create_tasks(_app_config->context, _app_config->batch_id, _current_iteration);
            
            printf("\n");
            
            continue;
        }
        
        // Got some work to do.
        newly_added_points = 0;
        
        // Load working set into memory.
        int64_t after_id = 0;
        after_id = working_set == NULL ? 0 : working_set->index;
        db_get_working_set(_app_config->context, &working_set, after_id);
        
        // Do work.
        printf("Doing work on point_id=%ld.\n", current_job->point_id);
        
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
        
        // Iterate over the working_set of points, and find the point
        // assigned to this checkout task.
        p1_node = working_set;
        while (((point_t*)p1_node->data)->point_id != current_job->point_id) {
            p1_node = p1_node->next;
            if (p1_node == NULL) {
                global_error_printf("Could not find point_id=%ld in working_set.\n", current_job->point_id);
                goto EXIT_LOOP;
            }
        }
        
        // Inner loop where the points are constructed.
        p1_count=0;
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
            newly_added_points += add_circle_x_line(_app_config->context, left_circle1, left_line);
            
            // (x2)
            newly_added_points += add_circle_x_line(_app_config->context, left_circle2, left_line);
            
            // (x3)
            newly_added_points += add_circle_x_circle(_app_config->context, left_circle1, left_circle2);
            
            // first pair covered, onto the second pair
            for (p3_node = p1_node, p3_count=0; p3_node != NULL; p3_node = p3_node->next, p3_count++) {
                for (p4_node = p3_node->next, p4_count=0; p4_node != NULL; p4_node = p4_node->next, p4_count++) {
                    
                    loop4_count++;
                    
                    if (p1_node == p3_node && p2_node == p4_node) {
                        continue;
                    }
                    
                    gettimeofday(&tv2, NULL);
                    total_elapsed = (double)(tv2.tv_sec - start_tv.tv_sec);
                    
                    // Check for benchmark to exit early.
                    if (_app_config->benchmark_time_sec > 0 
                                && total_elapsed > (double)(_app_config->benchmark_time_sec)) {
                        count = mysql_get_table_count(_app_config->context->connection, _app_config->context->db_table_name_known);
                        printf("%llu: p1=(%zu,%zu) p2=(%zu,%zu) p3=(%zu,%zu) p4=(%zu,%zu) working_set length=%zu, known_points=%zu\n"
                        "BENCHMARK_TIME_SEC exceeded, exiting.\n",
                            (long long unsigned)total_elapsed,
                            p1_count,
                            p1_node->index,
                            p2_count,
                            p2_node->index,
                            p3_count,
                            p3_node->index,
                            p4_count,
                            p4_node->index,
                            working_set->index,
                            count
                            );
                        goto EXIT_LOOP;
                    }
                    
                    // Check for status update.
                    if (_app_config->update_interval_sec > 0 
                                && (double)(tv2.tv_sec) > next_status_update_time) {
                        next_status_update_time = (double)(next_status_update_time) + (double)(_app_config->update_interval_sec);
                        
                        count = mysql_get_table_count(_app_config->context->connection, _app_config->context->db_table_name_known);
                        printf("%llu: p1=(%zu,%zu) p2=(%zu,%zu) p3=(%zu,%zu) p4=(%zu,%zu) working_set length=%zu, known_points=%zu\n",
                        (long long unsigned)total_elapsed,
                        p1_count,
                        p1_node->index,
                        p2_count,
                        p2_node->index,
                        p3_count,
                        p3_node->index,
                        p4_count,
                        p4_node->index,
                        working_set->index,
                        count
                        );
                    }
                    
                    // Check for checkpoint save.
                    if (_app_config->checkpoint_interval_sec > 0 
                                && (double)(tv2.tv_sec) > next_checkpoint_time) {
                        next_checkpoint_time = (double)(next_checkpoint_time) + (double)(_app_config->checkpoint_interval_sec);
                        
                        printf("(checkpoint)\n");
                        //count = mysql_get_table_count(_app_config->context->connection, _app_config->context->db_table_name_known);
                        //
                        //_save_context->current_iteration = _current_iteration;
                        //_save_context->p1_count = p1_count;
                        //_save_context->p1_index = p1_node->index;
                        //_save_context->p2_count = p2_count;
                        //_save_context->p2_index = p2_node->index;
                        //_save_context->p3_count = p3_count;
                        //_save_context->p3_index = p3_node->index;
                        //_save_context->p4_count = p4_count;
                        //_save_context->p4_index = p4_node->index;
                        //
                        //printf("%llu: p1=(%zu,%zu) p2=(%zu,%zu) p3=(%zu,%zu) p4=(%zu,%zu) working_set length=%zu, known_points=%zu\n",
                        //    (long long unsigned)total_elapsed,
                        //    _save_context->p1_count,
                        //    _save_context->p1_index,
                        //    _save_context->p2_count,
                        //    _save_context->p2_index,
                        //    _save_context->p3_count,
                        //    _save_context->p3_index,
                        //    _save_context->p4_count,
                        //    _save_context->p4_index,
                        //    working_set->index,
                        //    count
                        //    );
                        //
                        ////db_save_status(con, _save_context);

                        //mysql_commit(_app_config->context->connection->con);
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
                        newly_added_points += add_circle_x_circle(_app_config->context, left_circle1, right_circle2);
                    }
                    else if (p2 == p3)
                    {
                        // skip (1) left_line x right_line
                        // skip (6) left_circle1 x right_circle2
                        // (5)
                        newly_added_points += add_circle_x_circle(_app_config->context, left_circle1, right_circle1);
                    }
                    else
                    {
                        // (1)
                        newly_added_points += add_line_x_line(_app_config->context, left_line, right_line);
                        
                        // (5)
                        newly_added_points += add_circle_x_circle(_app_config->context, left_circle1, right_circle1);
                        
                        // (6)
                        newly_added_points += add_circle_x_circle(_app_config->context, left_circle1, right_circle2);
                    }

                    // always check everything else
                        
                    // (1)
                    // handled above
                        
                    // (2)
                    newly_added_points += add_circle_x_line(_app_config->context, right_circle1, left_line);
                    
                    // (3)
                    newly_added_points += add_circle_x_line(_app_config->context, right_circle2, left_line);

                    // (4)
                    newly_added_points += add_circle_x_line(_app_config->context, left_circle1, right_line);
                    
                    // (5)
                    // handled above
                    
                    // (6)
                    // handled above
                    
                    // (7)
                    newly_added_points += add_circle_x_line(_app_config->context, left_circle2, right_line);
                    
                    // (8)
                    newly_added_points += add_circle_x_circle(_app_config->context, left_circle2, right_circle1);
                        
                    // (9)
                    newly_added_points += add_circle_x_circle(_app_config->context, left_circle2, right_circle2);

                    // And self intersections for right points (three total)
                    
                    // (x1)
                    newly_added_points += add_circle_x_line(_app_config->context, right_circle1, right_line);
                    
                    // (x2)
                    newly_added_points += add_circle_x_line(_app_config->context, right_circle2, right_line);
                    
                    // (x3)
                    newly_added_points += add_circle_x_circle(_app_config->context, right_circle1, right_circle2);
                    
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

        // Done with work.
        db_checkin_work(_app_config->context, current_job);
        run_status_free(current_job);
        current_job = NULL;
                
        //db_context_commit(_app_config->context);
        
        if (_app_config->print_iteration_stats) {
            printf("results for iteration %d\n", _current_iteration);
            
            count = working_set->index + 1;
            printf("working_set count: %zu\n", count);
                        
            printf("new points this iteration: %zu\n", newly_added_points);
            
            count = mysql_get_table_count(_app_config->context->connection, _app_config->context->db_table_name_known);
            printf("db known points count: %zu\n", count);
            printf("\n");
        }
    }
    
EXIT_LOOP:

    gettimeofday(&tv2, NULL);
    total_elapsed = (double)(tv2.tv_sec - start_tv.tv_sec);
    
    // show results
    
    printf("loop4_count: %zu\n", loop4_count);
    printf("primary run time: %llu seconds.\n", (long long unsigned)total_elapsed);
    
    printf("\n");
       
    // done with program, cleanup, free memory.
    
    if (current_job != NULL) {
        free(current_job);
    }
        
    do {
        if (starting_set != NULL) {
            point_free(starting_set->data);
        }

        result = single_linked_list_remove(&starting_set);
    } while (1 == result);
    
    do {
        if (working_set != NULL) {
            point_free(working_set->data);
        }

        result = single_linked_list_remove(&working_set);
    } while (1 == result);
    
    //db_context_commit(_app_config->context);

    mpf_clear(d1);
    mpf_clear(d2);
    
    global_free();
    global_point_free();
    global_line_free();
    global_circle_free();
    global_datamodel_free();
    
    app_config_free(_app_config); // calls db_context_free which also closes connection
    
    printf("success.\n");
    
    return 0;
}