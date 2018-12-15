/*
* Application specific database structs and methods.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#ifndef __DATAMODEL_H__
#define __DATAMODEL_H__

#include <stdint.h>
#include <sys/time.h>
#include <mysql.h>

#include "mysql_common.h"
#include "point.h"
#include "list.h"

#define RUN_STATUS_ID_MYSQL_TYPE MYSQL_TYPE_LONGLONG
#define RUN_STATUS_CLIENT_ID_MYSQL_TYPE MYSQL_TYPE_SHORT
#define RUN_STATUS_BATCH_ID_MYSQL_TYPE MYSQL_TYPE_LONG
#define RUN_STATUS_IS_RUNNING_MYSQL_TYPE MYSQL_TYPE_TINY
#define RUN_STATUS_IS_DONE_MYSQL_TYPE MYSQL_TYPE_TINY
#define RUN_STATUS_HAS_ERROR_MYSQL_TYPE MYSQL_TYPE_TINY
#define RUN_STATUS_ERROR_INFO_MYSQL_TYPE MYSQL_TYPE_STRING
#define RUN_STATUS_START_TIME_MYSQL_TYPE MYSQL_TYPE_DATETIME
#define RUN_STATUS_END_TIME_MYSQL_TYPE MYSQL_TYPE_DATETIME
#define RUN_STATUS_POINT_ID_MYSQL_TYPE MYSQL_TYPE_LONGLONG
#define RUN_STATUS_ITERATION_MYSQL_TYPE MYSQL_TYPE_TINY

// database context for application
typedef struct db_context {
    char* db_table_name_working;
    char* db_table_name_known;
    char* db_table_name_status;
    int db_point_char_digits;
    
    mysql_connection_t* connection;
} db_context_t;

typedef struct run_status {
    // `id` BIGINT NOT NULL AUTO_INCREMENT
    int64_t id;
    
    // `client_id` SMALLINT NULL DEFAULT NULL
    int16_t client_id;
    
    // `batch_id` INT NOT NULL DEFAULT 0
    int32_t batch_id;
    
    // `is_running` TINYINT NOT NULL DEFAULT 0
    uint8_t is_running;
    
    // `is_done` TINYINT NOT NULL DEFAULT 0
    uint8_t is_done;
    
    // `has_error` TINYINT NOT NULL DEFAULT 0
    uint8_t has_error;
    
    // `error_info` VARCHAR(255) NULL
    char* error_info;
    
    // `start_time` DATETIME NULL
    MYSQL_TIME* start_time;
    
    // `end_time` DATETIME NULL
    MYSQL_TIME* end_time;
    
    // `point_id` BIGINT NOT NULL
    // FOREIGN KEY (point_id) REFERENCES points_working(id)
    int64_t point_id;
    
    // `iteration` TINYINT NOT NULL DEFAULT 0
    uint8_t iteration;
} run_status_t;

typedef struct root_batch_status {
    int8_t is_currently_running;
    int8_t any_incomplete;
    int8_t last_complete_iteration;
} root_batch_status_t;

/*
* Allocates memory for static command buffer.
*/
void global_datamodel_init();

/*
* Frees memory used by static variables.
*/
void global_datamodel_free();

/*
* Allocates memory for the context, then loads settings from ini.
*
* @filename: ini file to read values from.
*
* returns: pointer to new context.
*/
db_context_t* db_context_from_ini(char* filename);

/*
* Closes and frees underlying connection, frees memory in use by context.
*
* @context: context to free.
*/
void db_context_free(db_context_t* context);

/*
* Handler called by ini parser.
*/
int db_context_ini_parse_handler(void* config, const char* section, const char* name, const char* value);

/*
* Prints database context settings.
*
* @context: context to print.
*/
void db_context_printf(db_context_t* context);

/*
* Opens database connection.
*
* @context: context used to connect.
*/
void db_context_connect(db_context_t* context);

/*
* Calls commit on the underlying connection.
*/
void db_context_commit(db_context_t* context);

/*
* Inserts a point into the known set table.
*
* @context: database context.
* @p: point to add.
*
* returns: the number of points added.
*/
int db_insert_known_set(db_context_t* context, point_t* p);

/*
* Inserts many points into the known set table.
*
* @context: database context.
* @points: list of points to add.
*
* returns: the number of points added.
*/
int db_insert_many_known_set(db_context_t* context, single_linked_list_t* points);

/*
* Updates an existing run_status.
*
* @context: database context.
* @status: item to update.
*/
void db_update_run_status(db_context_t* context, run_status_t* status);

/*
* Loads the working set of points into a list.
*
* @context: context to use.
* @working_set: list to put points in.
* @after: points after this id will be returned, use zero for all.
*
* returns: the number of points found.
*/
size_t db_get_working_set(db_context_t* context, single_linked_list_t** working_set, int64_t after);

/*
* Copies points from the known table into the working table.
* Assumes this is only called by client_id 0.
*
* @context: context to use.
*/
void db_copy_known_to_working(db_context_t* context, uint8_t iteration);

/*
* Convenience function for root client to determine what to do.
* Evaluates current tasks and stores results in status.
*
* @context: database context.
* @batch_id: batch_id of related jobs.
* @status: results of queries.
*/
void db_get_root_batch_status(db_context_t* context, int batch_id, root_batch_status_t* status);

/*
* This should only be called by the root client.
* Creates new tasks in the database based on the working points.
*
* @context: database context.
* @batch_id: batch_id of related jobs.
* @iteration: iteration of associated tasks.
*
* returns: number of new tasks created.
*/
size_t db_create_tasks(db_context_t* context, int batch_id, int8_t iteration);

/*
* Acquires a lock on the run_status table, then checks out an item.
* When the job is checked out, client_id, is_running, start_time
* are automatically set. Allocates memory if there is work.
*
* @context: database context.
* @batch_id: batch_id of related jobs.
* @client_id: id of client requesting work.
*
* returns: pointer to newly allocated run_status_t, or null if there is
* no work to checkout.
*/
run_status_t* db_checkout_work(db_context_t* context, int batch_id, int16_t client_id);

/*
* Checks in a job to the database. The end_time is automatically set,
* as well as is_running and is_done.
*
* @context: database context.
* @status: job to checkin.
*/
void db_checkin_work(db_context_t* context, run_status_t* status);

/*
* Allocates memory for a run_status_t and memsets to zero.
* No memory is allocated for start_time, end_time, or error_info.
*
* returns: pointer to new run_status_t.
*/
run_status_t* run_status_alloc();

/*
* Frees memory in use by run_status_t.
*
* @status: object to free.
*/
void run_status_free(run_status_t* status);

/*
* Converts a time_t to a MYSQL_TIME.
*
* @ttime: Input time.
* @pmtime: Output time.
*/
void run_status_set_mysql_time(MYSQL_TIME* pmtime, time_t* ttime);

#endif