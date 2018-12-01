/*
* Application settings container.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "app_config.h"
#include "mysql_common.h"
#include "datamodel.h"

#define INI_SECTION_NAME "app"

/*
* Allocates memory for the config, then loads settings from ini.
*
* @filename: ini file to read values from.
*
* returns: pointer to new config.
*/
app_config_t* app_config_from_ini(char* filename) {
    app_config_t* config = malloc(sizeof(app_config_t));
    
    if (config == NULL)
    {
        fprintf(stderr, "Fatal error calling malloc for app_config_t.\n");
        exit(1);
    }
    
    memset(config, 0, sizeof(app_config_t));
    
    if (ini_parse(filename, app_config_ini_parse_handler, config) < 0) {
        fprintf(stderr, "Can't load '%s'\n", filename);
        exit(1);
    }
    
    config->context = db_context_from_ini(filename);
    
    return config;
}

/*
* Frees memory in use by config.
*
* @app_config_t: config to free.
*/
void app_config_free(app_config_t* config) {
    if (config == NULL) {
        return;
    }
    
    if (config->output_filename != NULL) {
        free(config->output_filename);
        config->output_filename = NULL;
    };
    
    if (config->str_init_epsilon != NULL) {
        free(config->str_init_epsilon);
        config->str_init_epsilon = NULL;
    };
    
    if (config->starting_points_file != NULL) {
        free(config->starting_points_file);
        config->starting_points_file = NULL;
    };
    
    db_context_free(config->context);
}

/*
* Handler called by ini parser.
*/
int app_config_ini_parse_handler(void* config, const char* section, const char* name, const char* value)
{
    app_config_t* pconfig = (app_config_t*)config;
    
    if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "CLIENT_ID") == 0) {
        pconfig->client_id = (uint16_t)atoi(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "BATCH_ID") == 0) {
        pconfig->batch_id = (uint16_t)atoi(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "GMP_PRECISION_BITS") == 0) {
        sscanf(value, "%zu", &(pconfig->gmp_precision_bits));
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "STR_POINT_DIGITS") == 0) {
        sscanf(value, "%zu", &(pconfig->str_point_digits));
    }  else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "PRINT_DIGITS") == 0) {
        sscanf(value, "%zu", &(pconfig->print_digits));
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "MAX_ITERATIONS") == 0) {
        sscanf(value, "%zu", &(pconfig->max_iterations));
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "PRINT_OBJECT_DESCRIPTION_IN_INTERSECTION_CHECK") == 0) {
        pconfig->print_object_description_in_intersection_check = atoi(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "PRINT_NUMBER_INTERSECTIONS_FOUND") == 0) {
        pconfig->print_number_intersections_found = atoi(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "PRINT_ITERATION_STATS") == 0) {
        pconfig->print_iteration_stats = atoi(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "UPDATE_INTERVAL_SEC") == 0) {
        pconfig->update_interval_sec = atoi(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "CHECKPOINT_INTERVAL_SEC") == 0) {
        pconfig->checkpoint_interval_sec = atoi(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "ALLOW_RESUME_FROM_CHECKPOINT") == 0) {
        pconfig->allow_resume_from_checkpoint = atoi(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "WRITE_POINTS_TO_FILE") == 0) {
        pconfig->write_points_to_file = atoi(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "OUTPUT_FILENAME") == 0) {
        pconfig->output_filename = strdup(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "STR_EPSILON") == 0) {
        pconfig->str_init_epsilon = strdup(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "STARTING_POINTS_FILE") == 0) {
        pconfig->starting_points_file = strdup(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "STARTING_POINTS_FILE_LINE_BUFFER") == 0) {
        sscanf(value, "%zu", &(pconfig->starting_points_file_line_buffer));
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "BENCHMARK_TIME_SEC") == 0) {
        sscanf(value, "%zu", &(pconfig->benchmark_time_sec));
    } else {
        return 0;  /* unknown section/name, error */
    }
    
    return 1;
}

/*
* Prints config settings.
*
* @config: config settings to print.
*/
void app_config_printf(app_config_t* config) {
    
    db_context_printf(config->context);
    
    printf("client_id: %d\n", config->client_id);
    printf("batch_id: %d\n", config->batch_id);
    printf("gmp_precision_bits: %zu\n", config->gmp_precision_bits);
    printf("str_point_digits: %zu\n", config->str_point_digits);
    printf("print_digits: %zu\n", config->print_digits);
    printf("max_iterations: %zu\n", config->max_iterations);
    printf("print_object_description_in_intersection_check: %d\n", config->print_object_description_in_intersection_check);
    printf("print_number_intersections_found: %d\n", config->print_number_intersections_found);
    printf("print_iteration_stats: %d\n", config->print_iteration_stats);
    printf("update_interval_sec: %d\n", config->update_interval_sec);
    printf("checkpoint_interval_sec: %d\n", config->checkpoint_interval_sec);
    printf("allow_resume_from_checkpoint: %d\n", config->allow_resume_from_checkpoint);
    printf("write_points_to_file: %d\n", config->write_points_to_file);
    printf("output_filename: %s\n", config->output_filename);
    printf("str_init_epsilon: %s\n", config->str_init_epsilon);
    printf("benchmark_time_sec: %zu\n", config->benchmark_time_sec);
}