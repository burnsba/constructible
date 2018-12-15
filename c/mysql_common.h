/*
* Common functions for mysql.
* Shouldn't reference current application types.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#ifndef __MYSQL_COMMON_H__
#define __MYSQL_COMMON_H__

#include <mysql.h>
#include <stdint.h>

typedef struct mysql_connection {
    char* db_server;
    char* db_user;
    char* db_password;
    char* db_database_name;
    
    MYSQL* con;
    
    int verbose_level;
} mysql_connection_t;

/*
* Allocates memory for the connection, then loads settings from ini.
*
* @filename: ini file to read values from.
*
* returns: pointer to new connection.
*/
mysql_connection_t* mysql_connection_from_ini(char* filename);

/*
* Closes connection, and frees any memory used.
*
* @mysql_connection_t: connection to free.
*/
void mysql_connection_free(mysql_connection_t*);

/*
* Handler called by ini parser.
*/
int mysql_connection_ini_parse_handler(void* config, const char* section, const char* name, const char* value);

/*
* Prints connection settings.
*
* @connection: settings to print.
*/
void mysql_connection_printf(mysql_connection_t* connection);

/*
* Executes a query to count the number of elements in a table and returns the result.
*
* @connection: mysql connection
* @table: table to count
*/
size_t mysql_get_table_count(mysql_connection_t* connection, char* table);

/*
* Executes a query to lock a specific table.
*
* @connection: mysql connection
* @table: table to lock
*/
void mysql_lock_table(mysql_connection_t* connection, char* table_name);

/*
* Executes a query to unlock all tables.
*
* @connection: mysql connection
*/
void mysql_unlock_tables(mysql_connection_t* connection);

/*
* Alters current connection to check foreign keys or not.
*
* @connection: mysql connection.
* @val: value to set, zero or one.
*/
void mysql_set_foreign_key_checks(mysql_connection_t* connection, uint8_t val);

/*
* Truncates table and sets AUTO_INCREMENT to 1.
*
* @connection: mysql connection
* @table: table to reset
*/
void mysql_reset_table(mysql_connection_t* connection, char* table);

/*
* Prints mysql error then exits application.
*
* @connection: mysql connection
*/
void mysql_exit_error_no_close(mysql_connection_t* connection);

/*
* Prints mysql error, closes the connection, then exits.
*
* @connection: mysql connection
*/
void mysql_exit_error(mysql_connection_t* connection);

/*
* Prints mysql statement error, closes the connection, then exits.
*
* @connection: mysql connection
*/
void mysql_stmt_exit_error(mysql_connection_t* connection, MYSQL_STMT *stmt);

/*
* Closes the connection, then exits.
*
* @connection: mysql connection
*/
void mysql_exit(mysql_connection_t* connection);

#endif
