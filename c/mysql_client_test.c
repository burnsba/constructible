#include <stdio.h>
#include <mysql.h>

int main()
{
    printf("MySQL client version: %s\n", mysql_get_client_info());
    return 0;
}