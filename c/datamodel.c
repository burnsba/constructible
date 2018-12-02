/*
* Application specific database structs and methods.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <mysql.h>

#include "mysql_common.h"
#include "global.h"
#include "point.h"
#include "list.h"
#include "datamodel.h"
#include "ini.h"

#define COMMAND_BUFFER_SIZE 1024
#define INI_SECTION_NAME "mysql_schema"

static char* _buffer = NULL;

static char _cone = 1;

// my_bool is defined by mysql
static my_bool* _pcone = &_cone;

/*
* Allocates memory for static command buffer.
*/
void global_datamodel_init() {
    _buffer = malloc(sizeof(char)*COMMAND_BUFFER_SIZE);
    
    global_exit_if_null(_buffer, "Could not allocate command buffer\n");
}

/*
* Frees memory used by static variables.
*/
void global_datamodel_free() {
    free(_buffer);
}

/*
* Allocates memory for the context, then loads settings from ini.
*
* @filename: ini file to read values from.
*
* returns: pointer to new context.
*/
db_context_t* db_context_from_ini(char* filename) {
    db_context_t* context = malloc(sizeof(db_context_t));
    
    global_exit_if_null(context, "Fatal error calling malloc for db_context_t.\n");
    
    memset(context, 0, sizeof(mysql_connection_t));
    
    if (ini_parse(filename, db_context_ini_parse_handler, context) < 0) {
        global_error_printf("Can't load '%s'\n", filename);
        exit(1);
    }
    
    context->connection = mysql_connection_from_ini(filename);
    
    return context;
}

/*
* Closes and frees underlying connection, frees memory in use by context.
*
* @context: context to free.
*/
void db_context_free(db_context_t* context) {
    if (context == NULL) {
        return;
    }
    
    if (context->db_table_name_working != NULL) {
        free(context->db_table_name_working);
        context->db_table_name_working = NULL;
    };
    
    if (context->db_table_name_known != NULL) {
        free(context->db_table_name_known);
        context->db_table_name_known = NULL;
    };
    
    if (context->db_table_name_status != NULL) {
        free(context->db_table_name_status);
        context->db_table_name_status = NULL;
    };
    
    mysql_connection_free(context->connection);
    context->connection = NULL;
}

/*
* Handler called by ini parser.
*/
int db_context_ini_parse_handler(void* config, const char* section, const char* name, const char* value) {
    db_context_t* pconfig = (db_context_t*)config;
    
    if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "DB_TABLE_NAME_WORKING") == 0) {
        pconfig->db_table_name_working = strdup(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "DB_TABLE_NAME_KNOWN") == 0) {
        pconfig->db_table_name_known = strdup(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "DB_TABLE_NAME_STATUS") == 0) {
        pconfig->db_table_name_status = strdup(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "DB_POINT_CHAR_DIGITS") == 0) {
        pconfig->db_point_char_digits = atoi(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "DB_POINT_DECIMAL_DIGITS_PRECISION") == 0) {
        pconfig->db_point_decimal_digits_precision = atoi(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "DB_POINT_DECIMAL_DIGITS_SCALE") == 0) {
        pconfig->db_point_decimal_digits_scale = atoi(value);
    } else {
        return 0;  /* unknown section/name, error */
    }
    
    return 1;
}

/*
* Prints database context settings.
*
* @context: context to print.
*/
void db_context_printf(db_context_t* context) {
    
    mysql_connection_printf(context->connection);
    
    printf("db_table_name_working: '%s'\n", context->db_table_name_working);
    printf("db_table_name_known: '%s'\n", context->db_table_name_known);
    printf("db_table_name_status: '%s'\n", context->db_table_name_status);
    printf("db_point_char_digits: '%d'\n", context->db_point_char_digits);
    printf("db_point_decimal_digits_precision: '%d'\n", context->db_point_decimal_digits_precision);
    printf("db_point_decimal_digits_scale: '%d'\n", context->db_point_decimal_digits_scale);
}

/*
* Opens database connection.
*
* @context: context used to connect.
*/
void db_context_connect(db_context_t* context) {
    context->connection->con = mysql_init(NULL);
    
    if (context->connection->con == NULL) {
        mysql_exit_error_no_close(context->connection);
    }
    
    if (mysql_real_connect(context->connection->con, 
            context->connection->db_server,
            context->connection->db_user,
            context->connection->db_password,
            context->connection->db_database_name, 
            0, 
            NULL, 
            0) == NULL) {
        mysql_exit_error(context->connection);
    }
}

/*
* Calls commit on the underlying connection.
*/
void db_context_commit(db_context_t* context) {
    mysql_commit(context->connection->con);
}

/*
* Inserts a point into the known set table.
*
* @context: database context.
* @p: point to add.
*
* returns: the number of points added.
*/
int db_insert_known_set(db_context_t* context, point_t* p) {
    
    MYSQL_STMT  *stmt;
    MYSQL_BIND  bind[4];
    size_t row_count = 0;
    size_t char_count = 0;
    
    point_ensure_hash(p);
    
    memset(_buffer, 0, COMMAND_BUFFER_SIZE);
    char_count = sprintf(_buffer, 
        "INSERT INTO `%s` (`x`,`y`,`xd`,`yd`) "
        "VALUES (?,?,CAST(? AS DECIMAL(%d,%d)),CAST(? AS DECIMAL(%d,%d))) "
        "ON DUPLICATE KEY UPDATE `id`=`id`;",
        context->db_table_name_known,
        context->db_point_decimal_digits_precision,
        context->db_point_decimal_digits_scale,
        context->db_point_decimal_digits_precision,
        context->db_point_decimal_digits_scale);
        
    if (context->connection->verbose_level == 1) {
        printf("execute: %s\n", _buffer);
    }
    
    stmt = mysql_stmt_init(context->connection->con);
    
    global_exit_if_null(stmt, "mysql_stmt_init(), out of memory\n");
    
    if (mysql_stmt_prepare(stmt, _buffer, char_count)) {
        mysql_stmt_exit_error(context->connection, stmt);
    }

    // bind parameters
    
    unsigned long input_length = context->db_point_char_digits;
        
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (char *)(p->str_x);
    bind[0].is_null = 0;
    bind[0].length = &input_length;
        
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char *)(p->str_y);
    bind[1].is_null = 0;
    bind[1].length = &input_length;
    
    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = (char *)(p->str_x);
    bind[2].is_null = 0;
    bind[2].length = &input_length;
    
    bind[3].buffer_type = MYSQL_TYPE_STRING;
    bind[3].buffer = (char *)(p->str_y);
    bind[3].is_null = 0;
    bind[3].length = &input_length;
    
    if (context->connection->verbose_level == 1) {
        for (int i=0; i<4; i++) {
            printf("?.%d: %s\n", i, (char *)bind[i].buffer);
        }
    }

    // done binding parameters
    
    if (mysql_stmt_bind_param(stmt, bind)) {
        mysql_stmt_exit_error(context->connection, stmt);
    }

    if (mysql_stmt_execute(stmt)) {
        mysql_stmt_exit_error(context->connection, stmt);
    }
    
    row_count = mysql_stmt_affected_rows(stmt);
        
    if (mysql_stmt_close(stmt)) {
        mysql_exit_error(context->connection);
    }
    
    // Insert will add either zero or one rows, so this is safe 
    // to cast down to int from size_t.
    return (int)row_count;
}

/*
* Updates an existing run_status (exlcuding point_id, iteration).
*
* @context: database context.
* @status: item to update.
*/
void db_update_run_status(db_context_t* context, run_status_t* status) {
    
    MYSQL_STMT  *stmt;
    MYSQL_BIND  bind[9];
    size_t char_count = 0;
    
    memset(_buffer, 0, COMMAND_BUFFER_SIZE);
    char_count = sprintf(_buffer, 
        "UPDATE `%s` SET "
        "`client_id`=?, "
        "`batch_id`=?, "
        "`is_running`=?, "
        "`is_done`=?, "
        "`has_error`=?, "
        "`error_info`=?, "
        "`start_time`=?, "
        "`end_time`=? "
        "WHERE `id`=?",
        context->db_table_name_status
        );
        
    if (context->connection->verbose_level == 1) {
        printf("execute: %s\n", _buffer);
    }
        
    stmt = mysql_stmt_init(context->connection->con);
    
    global_exit_if_null(stmt, "mysql_stmt_init(), out of memory\n");
    
    if (mysql_stmt_prepare(stmt, _buffer, char_count)) {
        mysql_stmt_exit_error(context->connection, stmt);
    }
    
    // bind parameters
    
    bind[0].buffer_type = RUN_STATUS_CLIENT_ID_MYSQL_TYPE;
    bind[0].buffer = (char *)&(status->client_id);
    bind[0].is_null = 0;
    bind[0].length = 0;
    
    bind[1].buffer_type = RUN_STATUS_BATCH_ID_MYSQL_TYPE;
    bind[1].buffer = (char *)&(status->batch_id);
    bind[1].is_null = 0;
    bind[1].length = 0;
    
    bind[2].buffer_type = RUN_STATUS_IS_RUNNING_MYSQL_TYPE;
    bind[2].buffer = (char *)&(status->is_running);
    bind[2].is_null = 0;
    bind[2].length = 0;
    
    bind[3].buffer_type = RUN_STATUS_IS_DONE_MYSQL_TYPE;
    bind[3].buffer = (char *)&(status->is_done);
    bind[3].is_null = 0;
    bind[3].length = 0;
    
    bind[4].buffer_type = RUN_STATUS_HAS_ERROR_MYSQL_TYPE;
    bind[4].buffer = (char *)&(status->has_error);
    bind[4].is_null = 0;
    bind[4].length = 0;
    
    if (status->error_info == NULL) {
        bind[5].buffer_type = MYSQL_TYPE_NULL;
        bind[5].is_null = _pcone;
    } else {
        unsigned long len = (unsigned long)strlen(status->error_info);
        
        bind[5].buffer_type = RUN_STATUS_ERROR_INFO_MYSQL_TYPE;
        // already a pointer
        bind[5].buffer = (char *)(status->error_info);
        bind[5].is_null = 0;
        bind[5].length = &len;
    }
    
    if (status->start_time == NULL) {
        bind[6].buffer_type = MYSQL_TYPE_NULL;
        bind[6].is_null = _pcone;
    } else {
        bind[6].buffer_type = RUN_STATUS_START_TIME_MYSQL_TYPE;
        // already a pointer
        bind[6].buffer = (char *)(status->start_time);
        bind[6].is_null = 0;
        bind[6].length = 0;
    }
    
    if (status->end_time == NULL) {
        bind[7].buffer_type = MYSQL_TYPE_NULL;
        bind[7].is_null = _pcone;
    } else {
        bind[7].buffer_type = RUN_STATUS_END_TIME_MYSQL_TYPE;
        // already a pointer
        bind[7].buffer = (char *)(status->end_time);
        bind[7].is_null = 0;
        bind[7].length = 0;
    }
    
    bind[8].buffer_type = RUN_STATUS_ID_MYSQL_TYPE;
    bind[8].buffer = (char *)&(status->id);
    bind[8].is_null = 0;
    bind[8].length = 0;
    
    // done binding parameters
    
    if (mysql_stmt_bind_param(stmt, bind)) {
        mysql_stmt_exit_error(context->connection, stmt);
    }

    if (mysql_stmt_execute(stmt)) {
        mysql_stmt_exit_error(context->connection, stmt);
    }
    
    mysql_stmt_affected_rows(stmt);
        
    if (mysql_stmt_close(stmt)) {
        mysql_exit_error(context->connection);
    }
}

/*
* Loads the working set of points into a list.
*
* @context: context to use.
* @working_set: list to put points in.
* @after: points after this id will be returned, use zero for all.
*
* returns: the number of points found.
*/
size_t db_get_working_set(db_context_t* context, single_linked_list_t** working_set, int64_t after) {
    int64_t point_id;
    
    memset(_buffer, 0, COMMAND_BUFFER_SIZE);
    sprintf(_buffer, 
        "SELECT `x`,`y`,`id` FROM `%s` "
        "WHERE `id` >= %ld "
        "ORDER BY `x`,`y`;",
        context->db_table_name_working,
        after);

    if (context->connection->verbose_level == 1) {
        printf("execute: %s\n", _buffer);
    }
    if (mysql_query(context->connection->con, _buffer)) {
        mysql_exit_error(context->connection);
    }
    
    MYSQL_RES *result = mysql_store_result(context->connection->con);
    
    if (result == NULL) {
        mysql_exit_error(context->connection);
    }
    
    MYSQL_ROW row;
    size_t row_count = 0;

    while ((row = mysql_fetch_row(result))) 
    {
        point_t* p1 = point_alloc();
        point_init(p1);
        point_set_str(p1, row[0], row[1]);
        
        sscanf(row[2], "%ld", &point_id);
        p1->point_id = point_id;
        
        single_linked_list_add(working_set, p1, sizeof(single_linked_list_t));
        row_count++;
    }
    
    mysql_free_result(result);
    
    return row_count;
}

/*
* Copies points from the known table into the working table.
* Assumes this is only called by client_id 0.
*
* @context: context to use.
*/
void db_copy_known_to_working(db_context_t* context, uint8_t iteration) {
    memset(_buffer, 0, COMMAND_BUFFER_SIZE);
    sprintf(_buffer, 
        "INSERT INTO `%s` (`x`,`y`,`xd`,`yd`,`iteration_origin`) "
        "SELECT `x`,`y`,`xd`,`yd`,%d FROM `%s` "
        "ON DUPLICATE KEY UPDATE `%s`.`id`=`%s`.`id`;",
        context->db_table_name_working,
        iteration,
        context->db_table_name_known,
        context->db_table_name_working,
        context->db_table_name_working);

    if (context->connection->verbose_level == 1) {
        printf("execute: %s\n", _buffer);
    }
    if (mysql_query(context->connection->con, _buffer)) {
        mysql_exit_error(context->connection);
    }
}

/*
* Convenience function for root client to determine what to do.
* Evaluates current tasks and stores results in status.
*
* @context: database context.
* @batch_id: batch_id of related jobs.
* @status: results of queries.
*/
void db_get_root_batch_status(db_context_t* context, int batch_id, root_batch_status_t* status) {
    MYSQL_RES *mysql_result;
    MYSQL_ROW row;
    int8_t result;
    
    memset(status, 0, sizeof(root_batch_status_t));
    
    // Get last_complete_iteration
    
    memset(_buffer, 0, COMMAND_BUFFER_SIZE);
    sprintf(_buffer, 
        "SELECT MAX(`iteration`) FROM `%s` "
        "WHERE `is_done` = 1 "
        "AND `batch_id` = %d",
        context->db_table_name_status,
        batch_id);

    if (context->connection->verbose_level == 1) {
        printf("execute: %s\n", _buffer);
    }
    if (mysql_query(context->connection->con, _buffer)) {
        mysql_exit_error(context->connection);
    }

    mysql_result = mysql_store_result(context->connection->con);
    
    if (mysql_result == NULL) {
        mysql_exit_error(context->connection);
    }
    
    row = mysql_fetch_row(mysql_result);
    
    if (row == NULL || row[0] == NULL) {
        result = (int8_t)(-1);
    } else {
        // hhd = signed char
        sscanf(row[0], "%hhd", &result);
    }
    
    status->last_complete_iteration = result;
    
    mysql_free_result(mysql_result);
    
    // done getting last_complete_iteration
    
    // get is_currently_running
    
    memset(_buffer, 0, COMMAND_BUFFER_SIZE);
    sprintf(_buffer, 
        "SELECT MAX(`id`) FROM `%s` "
        "WHERE `is_done` = 0 "
        "AND `is_running` = 1 "
        "AND `batch_id` = %d",
        context->db_table_name_status,
        batch_id);

    if (context->connection->verbose_level == 1) {
        printf("execute: %s\n", _buffer);
    }
    if (mysql_query(context->connection->con, _buffer)) {
        mysql_exit_error(context->connection);
    }

    mysql_result = mysql_store_result(context->connection->con);
    
    if (mysql_result == NULL) {
        mysql_exit_error(context->connection);
    }
    
    row = mysql_fetch_row(mysql_result);
    
    if (row == NULL || row[0] == NULL) {
        result = 0;
    } else {
        result = 1;
    }
    
    status->is_currently_running = result;
    
    mysql_free_result(mysql_result);
    
    // done getting is_currently_running
    
    // get any_incomplete
    
    memset(_buffer, 0, COMMAND_BUFFER_SIZE);
    sprintf(_buffer, 
        "SELECT MAX(`id`) FROM `%s` "
        "WHERE `is_done` = 0 "
        "AND `batch_id` = %d",
        context->db_table_name_status,
        batch_id);

    if (context->connection->verbose_level == 1) {
        printf("execute: %s\n", _buffer);
    }
    if (mysql_query(context->connection->con, _buffer)) {
        mysql_exit_error(context->connection);
    }

    mysql_result = mysql_store_result(context->connection->con);
    
    if (mysql_result == NULL) {
        mysql_exit_error(context->connection);
    }
    
    row = mysql_fetch_row(mysql_result);
    
    if (row == NULL || row[0] == NULL) {
        result = 0;
    } else {
        result = 1;
    }
    
    status->any_incomplete = result;
    
    mysql_free_result(mysql_result);
    
    // done getting any_incomplete
}

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
size_t db_create_tasks(db_context_t* context, int batch_id, int8_t iteration) {
    MYSQL_STMT  *stmt;
    MYSQL_BIND  bind[2];
    size_t row_count = 0;
    size_t char_count = 0;
        
    memset(_buffer, 0, COMMAND_BUFFER_SIZE);
    char_count = sprintf(_buffer, 
        "INSERT INTO `%s` (`batch_id`,`point_id`,`iteration`) "
        "SELECT ?,`id`,? FROM `%s`",
        context->db_table_name_status,
        context->db_table_name_working);
        
    if (context->connection->verbose_level == 1) {
        printf("execute: %s\n", _buffer);
    }
    
    stmt = mysql_stmt_init(context->connection->con);
    
    global_exit_if_null(stmt, "mysql_stmt_init(), out of memory\n");
    
    if (mysql_stmt_prepare(stmt, _buffer, char_count)) {
        mysql_stmt_exit_error(context->connection, stmt);
    }

    // bind parameters
    bind[0].buffer_type = RUN_STATUS_BATCH_ID_MYSQL_TYPE;
    bind[0].buffer = (char *)&(batch_id);
    bind[0].is_null = 0;
    bind[0].length = 0;
    
    bind[1].buffer_type = RUN_STATUS_ITERATION_MYSQL_TYPE;
    bind[1].buffer = (char *)&(iteration);
    bind[1].is_null = 0;
    bind[1].length = 0;
    
    // done binding parameters
    
    if (mysql_stmt_bind_param(stmt, bind)) {
        mysql_stmt_exit_error(context->connection, stmt);
    }

    if (mysql_stmt_execute(stmt)) {
        mysql_stmt_exit_error(context->connection, stmt);
    }
    
    row_count = mysql_stmt_affected_rows(stmt);
        
    if (mysql_stmt_close(stmt)) {
        mysql_exit_error(context->connection);
    }
    
    return row_count;
}

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
run_status_t* db_checkout_work(db_context_t* context, int batch_id, int16_t client_id) {
    
    int64_t id, point_id;
    MYSQL_RES *mysql_result;
    MYSQL_ROW row;
    run_status_t *result = NULL;
    
    memset(_buffer, 0, COMMAND_BUFFER_SIZE);
    sprintf(_buffer, 
        "LOCK TABLES `%s` WRITE;",
        context->db_table_name_status);

    if (context->connection->verbose_level == 1) {
        printf("execute: %s\n", _buffer);
    }
    if (mysql_query(context->connection->con, _buffer)) {
        mysql_exit_error(context->connection);
    }
    
    // do stuff
    
    memset(_buffer, 0, COMMAND_BUFFER_SIZE);
    sprintf(_buffer, 
        "SELECT `id`,`point_id` FROM `%s` "
        "WHERE `client_id` IS NULL "
        "AND `batch_id` = %d "
        "ORDER BY `point_id` "
        "LIMIT 1;",
        context->db_table_name_status,
        batch_id);

    if (context->connection->verbose_level == 1) {
        printf("execute: %s\n", _buffer);
    }
    if (mysql_query(context->connection->con, _buffer)) {
        mysql_exit_error(context->connection);
    }
    
    mysql_result = mysql_store_result(context->connection->con);
    
    if (mysql_result == NULL) {
        mysql_exit_error(context->connection);
    }
    
    row = mysql_fetch_row(mysql_result);
    
    if (row != NULL) {
        // There are assignments that can be completed.
        id = 0;
        point_id = 0;
        sscanf(row[0], "%ld", &id);
        sscanf(row[1], "%ld", &point_id);
        mysql_free_result(mysql_result);
        
        result = run_status_alloc();
        
        result->id = id;
        result->batch_id = batch_id;
        result->client_id = client_id;
        result->is_running = 1;
        result->point_id = point_id;
        
        result->start_time = malloc(sizeof(MYSQL_TIME));
        global_exit_if_null(result->start_time, "Could not allocate space for start_time\n");
        memset(result->start_time, 0, sizeof(MYSQL_TIME));
        
        time_t now = time(NULL);
        run_status_set_mysql_time(result->start_time, &now);
        
        db_update_run_status(context, result);
    } else {
        mysql_free_result(mysql_result);
    }
    
    // done.
    
    memset(_buffer, 0, COMMAND_BUFFER_SIZE);
    sprintf(_buffer, "UNLOCK TABLES;");

    if (context->connection->verbose_level == 1) {
        printf("execute: %s\n", _buffer);
    }
    if (mysql_query(context->connection->con, _buffer)) {
        mysql_exit_error(context->connection);
    }
    
    return result;
}

/*
* Checks in a job to the database. The end_time is automatically set,
* as well as is_running and is_done.
*
* @context: database context.
* @status: job to checkin.
*/
void db_checkin_work(db_context_t* context, run_status_t* status) {
    assert(status->start_time != NULL);
    
    status->is_running = 0;
    status->is_done = 1;
    
    if (status->end_time == NULL) {
        status->end_time = malloc(sizeof(MYSQL_TIME));
        global_exit_if_null(status->end_time, "Could not allocate space for end_time\n");
        memset(status->end_time, 0, sizeof(MYSQL_TIME));
    }
    
    time_t now = time(NULL);
    run_status_set_mysql_time(status->end_time, &now);
    
    db_update_run_status(context, status);
}

/*
* Allocates memory for a run_status_t and memsets to zero.
* No memory is allocated for start_time, end_time, or error_info.
*
* returns: pointer to new run_status_t.
*/
run_status_t* run_status_alloc() {
    run_status_t* status = malloc(sizeof(run_status_t));
    global_exit_if_null(status, "Fatal error calling malloc for run_status_t.\n");
    memset(status, 0, sizeof(run_status_t));
    
    return status;
}

/*
* Frees memory in use by run_status_t.
*
* @status: object to free.
*/
void run_status_free(run_status_t* status) {
    if (status == NULL) {
        return;
    }
    
    if (status->error_info != NULL) {
        free(status->error_info);
        status->error_info = NULL;
    }
    
    if (status->start_time != NULL) {
        free(status->start_time);
        status->start_time = NULL;
    }
    
    if (status->end_time != NULL) {
        free(status->end_time);
        status->end_time = NULL;
    }
    
    free(status);
}

/*
* Converts a time_t to a MYSQL_TIME.
*
* @ttime: Input time.
* @pmtime: Output time.
*/
void run_status_set_mysql_time(MYSQL_TIME* pmtime, time_t* ttime) {
    struct tm * timeinfo;
    timeinfo = localtime(ttime);
    
    pmtime->year = timeinfo->tm_year + 1900;
    pmtime->month = timeinfo->tm_mon;
    pmtime->day = timeinfo->tm_mday;

    pmtime->hour = timeinfo->tm_hour;
    pmtime->minute = timeinfo->tm_min;
    pmtime->second = timeinfo->tm_sec;
}
