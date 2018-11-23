CC=gcc
CFLAGS=-g -Wall -Wextra -Werror=implicit-function-declaration
LIBS=-lgmp
MYSQL_CFLAGS=$(shell mysql_config --cflags)
MYSQL_LIBS=$(shell mysql_config --libs)

all: constructible mysql_schema
ub: upper_bound
mysql: mysql_client_test mysql_schema

# make executables

mysql_client_test: mysql_client_test.c
	$(CC) $(CFLAGS) $(MYSQL_CFLAGS) mysql_client_test.c -o mysql_client_test $(LIBS) $(MYSQL_LIBS)

upper_bound: upper_bound.c
	$(CC) $(CFLAGS) upper_bound.c -o upper_bound $(LIBS)

constructible: constructible.o test.o global.o circle.o line.o point.o list.o mysql_common.o ini.o app_config.o datamodel.o
	$(CC) $(CFLAGS) $(MYSQL_CFLAGS) constructible.o test.o global.o circle.o line.o point.o list.o mysql_common.o ini.o app_config.o datamodel.o -o constructible $(LIBS) $(MYSQL_LIBS)

# the application specific database context (datamodel) depends on point and list.
mysql_schema: mysql_common.o mysql_schema.o ini.o global.o datamodel.o point.o list.o
	$(CC) $(CFLAGS) $(MYSQL_CFLAGS) mysql_schema.o mysql_common.o global.o ini.o datamodel.o point.o list.o -o mysql_schema $(LIBS) $(MYSQL_LIBS)

# make objects

mysql_schema.o: mysql_schema.c mysql_common.o
	$(CC) $(CFLAGS) $(MYSQL_CFLAGS) -c mysql_schema.c $(LIBS) $(MYSQL_LIBS)

mysql_common.o: mysql_common.c
	$(CC) $(CFLAGS) $(MYSQL_CFLAGS) -c mysql_common.c $(LIBS) $(MYSQL_LIBS)

constructible.o: point.o constructible.c
	$(CC) $(CFLAGS) $(MYSQL_CFLAGS) -c constructible.c $(LIBS) $(MYSQL_LIBS)

app_config.o: app_config.c
	$(CC) $(CFLAGS) $(MYSQL_CFLAGS) -c app_config.c $(LIBS) $(MYSQL_LIBS)

datamodel.o: datamodel.c
	$(CC) $(CFLAGS) $(MYSQL_CFLAGS) -c datamodel.c $(LIBS) $(MYSQL_LIBS)

circle.o: circle.c
	$(CC) $(CFLAGS) -c circle.c $(LIBS)

line.o: line.c
	$(CC) $(CFLAGS) -c line.c $(LIBS)

point.o: 
	$(CC) $(CFLAGS) -c point.c $(LIBS)

test.o: test.c
	$(CC) $(CFLAGS) -c test.c $(LIBS)

list.o: list.c
	$(CC) $(CFLAGS) -c list.c $(LIBS)

global.o: global.c
	$(CC) $(CFLAGS) -c global.c $(LIBS)

ini.o:
	$(CC) $(CFLAGS) -c ini.c

# clean 
clean:
	rm -f *.o *.exe constructible upper_bound mysql_client_test mysql_schema
