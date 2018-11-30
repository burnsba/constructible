/*
* Common functions for mysql.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <string.h>
#include <stdint.h>

#include "global.h"
#include "mysql_common.h"
#include "ini.h"

#define STACK_BUFFER_SIZE 512

#define INI_SECTION_NAME "mysql"

/*
* Allocates memory for the connection, then loads settings from ini.
*
* @filename: ini file to read values from.
*
* returns: pointer to new connection.
*/
mysql_connection_t* mysql_connection_from_ini(char* filename) {
    mysql_connection_t* connection = malloc(sizeof(mysql_connection_t));
    
    global_exit_if_null(connection, "Fatal error calling malloc for mysql_connection_t.\n");
    
    memset(connection, 0, sizeof(mysql_connection_t));
    
    if (ini_parse(filename, mysql_connection_ini_parse_handler, connection) < 0) {
        global_error_printf("Can't load '%s'\n", filename);
        exit(1);
    }
    
    return connection;
}

/*
* Closes connection, and frees any memory used.
*
* @connection: connection to free.
*/
void mysql_connection_free(mysql_connection_t* connection) {
    if (connection == NULL) {
        return;
    }
    
    if (connection->db_server != NULL) {
        free(connection->db_server);
        connection->db_server = NULL;
    };

    if (connection->db_user != NULL) {
        free(connection->db_user);
        connection->db_user = NULL;
    };
    
    if (connection->db_password != NULL) {
        free(connection->db_password);
        connection->db_password = NULL;
    };
    
    if (connection->db_database_name != NULL) {
        free(connection->db_database_name);
        connection->db_database_name = NULL;
    };
    
    mysql_close(connection->con);
    connection->con = NULL;
}

/*
* Handler called by ini parser.
*/
int mysql_connection_ini_parse_handler(void* config, const char* section, const char* name, const char* value) {
    mysql_connection_t* pconfig = (mysql_connection_t*)config;
    
    if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "DB_SERVER") == 0) {
        pconfig->db_server = strdup(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "DB_USER") == 0) {
        pconfig->db_user = strdup(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "DB_PASSWORD") == 0) {
        pconfig->db_password = strdup(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "DB_DATABASE_NAME") == 0) {
        pconfig->db_database_name = strdup(value);
    } else if (strcmp(section, INI_SECTION_NAME) == 0 && strcmp(name, "PRINT_SQL_COMMAND") == 0) {
        pconfig->verbose_level = atoi(value);
    } else {
        return 0;  /* unknown section/name, error */
    }
    
    return 1;
}

/*
* Prints connection settings.
*
* @connection: settings to print.
*/
void mysql_connection_printf(mysql_connection_t* connection) {
    printf("db_server: '%s'\n", connection->db_server);
    printf("db_user: '%s'\n", connection->db_user);
    printf("db_password: '%s'\n", "*" );
    printf("db_database_name: '%s'\n", connection->db_database_name);
    printf("verbose_level: %d\n", connection->verbose_level);
}

/*
* Executes a query to count the number of elements in a table and returns the result.
*
* @connection: mysql connection
* @table: table to count
*/
size_t mysql_get_table_count(mysql_connection_t* connection, char* table) {
    char buffer[STACK_BUFFER_SIZE];
    
    size_t row_count = 0;
    
    memset(buffer, 0, STACK_BUFFER_SIZE);
    sprintf(buffer, "SELECT COUNT(*) FROM `%s`;", table);

    if (connection->verbose_level == 1) {
        printf("execute: %s\n", buffer);
    }
    if (mysql_query(connection->con, buffer)) {
        mysql_exit_error(connection);
    }

    MYSQL_RES *result = mysql_store_result(connection->con);
    
    if (result == NULL) {
        mysql_exit_error(connection);
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    
    if (row == NULL || row[0] == NULL) {
        row_count = 0;
    } else {
        sscanf(row[0], "%zu", &row_count);
    }
    
    mysql_free_result(result);
    
    return row_count;
}

/*
* Alters current connection to check foreign keys or not.
*
* @connection: mysql connection.
* @val: value to set, zero or one.
*/
void mysql_set_foreign_key_checks(mysql_connection_t* connection, uint8_t val) {
    char buffer[STACK_BUFFER_SIZE];
    
    memset(buffer, 0, STACK_BUFFER_SIZE);
    sprintf(buffer, "SET FOREIGN_KEY_CHECKS = %d;", (val&0x01));

    if (connection->verbose_level == 1) {
        printf("execute: %s\n", buffer);
    }
    if (mysql_query(connection->con, buffer)) {
        mysql_exit_error(connection);
    }
}

/*
* Truncates table and sets AUTO_INCREMENT to 1.
*
* @connection: mysql connection
* @table: table to reset
*/
void mysql_reset_table(mysql_connection_t* connection, char* table) {
    char buffer[STACK_BUFFER_SIZE];
    
    memset(buffer, 0, STACK_BUFFER_SIZE);
    sprintf(buffer, "TRUNCATE TABLE `%s`;", table);

    if (connection->verbose_level == 1) {
        printf("execute: %s\n", buffer);
    }
    if (mysql_query(connection->con, buffer)) {
        mysql_exit_error(connection);
    }
    
    memset(buffer, 0, STACK_BUFFER_SIZE);
    sprintf(buffer, "ALTER TABLE `%s` AUTO_INCREMENT = 1;", table);

    if (connection->verbose_level == 1) {
        printf("execute: %s\n", buffer);
    }
    if (mysql_query(connection->con, buffer)) {
        mysql_exit_error(connection);
    }
}

/*
* Prints mysql error then exits application.
*
* @connection: mysql connection
*/
void mysql_exit_error_no_close(mysql_connection_t* connection) {
    global_error_printf("%s\n", mysql_error(connection->con));
    exit(1);
}

/*
* Prints mysql error, closes the connection, then exits.
*
* @connection: mysql connection
*/
void mysql_exit_error(mysql_connection_t* connection) {
    global_error_printf("%s\n", mysql_error(connection->con));
    mysql_close(connection->con);
    exit(1);
}

/*
* Prints mysql statement error, closes the connection, then exits.
*
* @connection: mysql connection
*/
void mysql_stmt_exit_error(mysql_connection_t* connection, MYSQL_STMT *stmt) {
    global_error_printf("%s\n", mysql_stmt_error(stmt));
    mysql_close(connection->con);
    exit(1);
}

/*
* Closes the connection, then exits.
*
* @connection: mysql connection
*/
void mysql_exit(mysql_connection_t* connection) {
    mysql_close(connection->con);
    exit(1);
}
