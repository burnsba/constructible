/*
* Application settings container.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include <stdint.h>

#include "app_config.h"
#include "mysql_common.h"
#include "datamodel.h"
#include "ini.h"

typedef struct app_config {
    db_context_t* context;
    
    // Distributed client ids, these need to be unique.
    // Id 0 gets special privileges.
    uint16_t client_id;
    
    // Distributed clients will work on the same batch.
    uint16_t batch_id;
    
    // Parameter passed to GMP to set the default precision.
    // default: 200
    size_t gmp_precision_bits;
    
    // This number should be the base 10 representation of the number of significant
    // digits, which should be ~ ln(2^PRECISION_BITS)/ln(10).
    size_t str_point_digits;
    
    // Number of decimal digits to use when printing output. This is smaller than
    // the above to avoid extra clutter.
    size_t print_digits;
    
    // Will attempt to construct points only this many times.
    size_t max_iterations;
    
    // When iterating over objects, enabling this will describe what is 
    // being checked. Output like:
    // check {0.0000000000, 0.0000000000} -> {1.0000000000} x {0.0000000000, 1.0000000000} -> {1.0000000000}
    int print_object_description_in_intersection_check;
    
    // When iterating over objects, enabling will print how many points
    // where found for that intersect. Output like:
    // 2 intersections found.
    int print_number_intersections_found;
    
    // Enabling this will print out some numbers at the end of each iteration.
    int print_iteration_stats;
    
    // If more than this many seconds have passed since the last status 
    // update, write a status update in the inner p3/p4 loop.
    // Set to zero or negative to disable
    int update_interval_sec;
    
    // If more than this many seconds have passed since the last
    // checkpoint, save a new checkpoint. Set to zero or negative to disable.
    int checkpoint_interval_sec;
    
    // If set, will attempt to load previous state from database and resume 
    // from there. Set to 0 to ignore and load points from starting file.
    int allow_resume_from_checkpoint;
    
    // After everything is done, sort the points and write the output to a file.
    // This file is truncated and overwritten.
    int write_points_to_file;
    char* output_filename;
    
    // absolute values less than this will be considered zero
    char* str_init_epsilon;
    
    // Read initial starting points from here
    char *starting_points_file;
    
    // Buffer size when reading starting points from file.
    size_t starting_points_file_line_buffer;
    
    // Abort if the application has been running longer than this many seconds.
    // Set to less than one to disable.
    size_t benchmark_time_sec;
    
    // Number of times to call db_insert_known_set before committing the 
    // current transaction.
    size_t queries_between_commits;
    
    // Whether or not to show commit message.
    int show_when_commit;
} app_config_t;

/*
* Allocates memory for the config, then loads settings from ini.
*
* @filename: ini file to read values from.
*
* returns: pointer to new config.
*/
app_config_t* app_config_from_ini(char* filename);

/*
* Frees memory in use by config.
*
* @app_config_t: config to free.
*/
void app_config_free(app_config_t*);

/*
* Handler called by ini parser.
*/
int app_config_ini_parse_handler(void* config, const char* section, const char* name, const char* value);

/*
* Prints config settings.
*
* @config: config settings to print.
*/
void app_config_printf(app_config_t* config);

#endif