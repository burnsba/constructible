/*
* Creates mysql database and table used by constructible.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include "console.h"
#include "mysql_common.h"
#include "ini.h"
#include "datamodel.h"

#define COMMAND_BUFFER_SIZE 1024
#define COMMAND_FILE_BUFFER_SIZE 4096

int main()
{
    char* command = malloc(sizeof(char)*COMMAND_BUFFER_SIZE);
    char* command_file = malloc(sizeof(char)*COMMAND_FILE_BUFFER_SIZE);
    
    db_context_t* context = db_context_from_ini("config.ini");
    
    printf("read config.ini\n");
    db_context_printf(context);
        
    db_context_connect(context);
    
    memset(command, 0, COMMAND_BUFFER_SIZE);
    sprintf(command, "CREATE DATABASE IF NOT EXISTS %s;", context->connection->db_database_name);
    printf("execute: %s\n", command);
    if (mysql_query(context->connection->con, command)) {
        mysql_exit_error(context->connection);
    }

    printf ("using database: %s\n", context->connection->db_database_name);
    if (mysql_select_db(context->connection->con, context->connection->db_database_name)) {
        mysql_exit_error(context->connection);
    }
    
    // foreign key to points_working so delete this first.
    memset(command, 0, COMMAND_BUFFER_SIZE);
    sprintf(command, "DROP TABLE IF EXISTS %s", context->db_table_name_status);
    printf("execute: %s\n", command);
    if (mysql_query(context->connection->con, command)) {
        mysql_exit_error(context->connection);
    }
    
    memset(command, 0, COMMAND_BUFFER_SIZE);
    sprintf(command, "DROP TABLE IF EXISTS %s", context->db_table_name_working);
    printf("execute: %s\n", command);
    if (mysql_query(context->connection->con, command)) {
        mysql_exit_error(context->connection);
    }
    
    memset(command, 0, COMMAND_BUFFER_SIZE);
    sprintf(command, "DROP TABLE IF EXISTS %s", context->db_table_name_known);
    printf("execute: %s\n", command);
    if (mysql_query(context->connection->con, command)) {
        mysql_exit_error(context->connection);
    }
       
    
    memset(command, 0, COMMAND_BUFFER_SIZE);
    sprintf(command, 
        "CREATE TABLE `%s` ("
        "`id` BIGINT NOT NULL AUTO_INCREMENT, "
        "`x` CHAR(%d) ASCII NOT NULL, "
        "`y` CHAR(%d) ASCII NOT NULL, "
        "`xd` DECIMAL(%d,%d) NOT NULL, "
        "`yd` DECIMAL(%d,%d) NOT NULL, "
        "`iteration_origin` TINYINT NOT NULL, "
        "PRIMARY KEY (`id`), "
        "INDEX `%s_idx_xd` (`xd`), "
        "INDEX `%s_idx_yd` (`yd`), "
        "UNIQUE (xd,yd)"
        ");",
        context->db_table_name_working,
        context->db_point_char_digits,
        context->db_point_char_digits,
        context->db_point_decimal_digits_precision,
        context->db_point_decimal_digits_scale,
        context->db_point_decimal_digits_precision,
        context->db_point_decimal_digits_scale,
        context->db_table_name_working,
        context->db_table_name_working);
    printf("execute: %s\n", command);    
    if (mysql_query(context->connection->con, command)) {
        mysql_exit_error(context->connection);
    }
    
    memset(command, 0, COMMAND_BUFFER_SIZE);
    sprintf(command, 
        "CREATE TABLE `%s` ("
        "`id` BIGINT NOT NULL AUTO_INCREMENT, "
        "`x` CHAR(%d) ASCII NOT NULL, "
        "`y` CHAR(%d) ASCII NOT NULL, "
        "`xd` DECIMAL(%d,%d) NOT NULL, "
        "`yd` DECIMAL(%d,%d) NOT NULL, "
        "PRIMARY KEY (`Id`), "
        "INDEX `%s_idx_xd` (`xd`), "
        "INDEX `%s_idx_yd` (`yd`), "
        "UNIQUE (xd,yd)"
        ");",
        context->db_table_name_known,
        context->db_point_char_digits,
        context->db_point_char_digits,
        context->db_point_decimal_digits_precision,
        context->db_point_decimal_digits_scale,
        context->db_point_decimal_digits_precision,
        context->db_point_decimal_digits_scale,
        context->db_table_name_known,
        context->db_table_name_known);
    printf("execute: %s\n", command);    
    if (mysql_query(context->connection->con, command)) {
        mysql_exit_error(context->connection);
    }
    
    memset(command, 0, COMMAND_BUFFER_SIZE);
    sprintf(command, 
        "CREATE TABLE `%s` ("
        "`id` BIGINT NOT NULL AUTO_INCREMENT, "
        "`client_id` SMALLINT NULL DEFAULT NULL, "
        "`batch_id` INT NOT NULL DEFAULT 0, "
        "`is_running` TINYINT NOT NULL DEFAULT 0, "
        "`is_done` TINYINT NOT NULL DEFAULT 0, "
        "`has_error` TINYINT NOT NULL DEFAULT 0, "
        "`error_info` VARCHAR(255) NULL, "
        "`start_time` DATETIME NULL, "
        "`end_time` DATETIME NULL, "
        "`point_id` BIGINT NOT NULL, " 
        "`iteration` TINYINT NOT NULL DEFAULT 0, "
        "PRIMARY KEY (`id`), "
        "FOREIGN KEY (point_id) REFERENCES %s(id) "
        ");",
        context->db_table_name_status,
        context->db_table_name_working);
    printf("execute: %s\n", command);    
    if (mysql_query(context->connection->con, command)) {
        mysql_exit_error(context->connection);
    }
    
    memset(command, 0, COMMAND_BUFFER_SIZE);
    sprintf(command, "DROP PROCEDURE IF EXISTS `consolidate_points`");
    printf("execute: %s\n", command);    
    if (mysql_query(context->connection->con, command)) {
        mysql_exit_error(context->connection);
    }
    
    memset(command_file, 0, COMMAND_FILE_BUFFER_SIZE);
    FILE *fp = fopen("consolidate_points.sql", "r");
    if (fp != NULL) {
        size_t read_len = fread(command_file, sizeof(char), COMMAND_FILE_BUFFER_SIZE - 2, fp);
        if (ferror(fp) != 0) {
            fprintf(stderr, "Error reading file\n");
            mysql_exit(context->connection);
        }
        printf("Read %zu bytes from consolidate_points.sql\n", read_len);
        fclose(fp);
    } else {
        fprintf(stderr, "Could not open consolidate_points.sql for read.\n");
        mysql_exit(context->connection);
    }
    command_file[COMMAND_FILE_BUFFER_SIZE - 1] = 0;
    
    printf("creating stored procedure consolidate_points\n");
    if (mysql_query(context->connection->con, command_file)) {
        mysql_exit_error(context->connection);
    }
    
    free(command);
    free(command_file);
    db_context_free(context); // also closes connection
    
    printf("done.\n");
    
    return 0;    
}