CC=gcc
CFLAGS=-c -Wall

all: puny

puny: puny.o
	$(CC) puny.o -o puny

puny.o: puny.c
	$(CC) $(CFLAGS) puny.c

clean:
	rm *o puny
