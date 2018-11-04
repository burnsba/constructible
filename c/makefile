CC=gcc
CFLAGS=-g -Wall -Wextra -Werror=implicit-function-declaration
LIBS=-lgmp

all: constructible
ub: upper_bound

upper_bound: upper_bound.c
	$(CC) $(CFLAGS) upper_bound.c -o upper_bound $(LIBS)

constructible: constructible.o test.o global.o circle.o line.o point.o list.o
	$(CC) $(CFLAGS) constructible.o test.o global.o circle.o line.o point.o list.o -o constructible $(LIBS)

constructible.o: constructible.c
	$(CC) $(CFLAGS) -c constructible.c $(LIBS)

circle.o: circle.c
	$(CC) $(CFLAGS) -c circle.c $(LIBS)

line.o: line.c
	$(CC) $(CFLAGS) -c line.c $(LIBS)

point.o: point.c
	$(CC) $(CFLAGS) -c point.c $(LIBS)

test.o: test.c
	$(CC) $(CFLAGS) -c test.c $(LIBS)

list.o: list.c
	$(CC) $(CFLAGS) -c list.c $(LIBS)

global.o: global.c
	$(CC) $(CFLAGS) -c global.c $(LIBS)

clean:
	rm *.o constructible upper_bound
