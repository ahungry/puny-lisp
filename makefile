CC=gcc
CFLAGS=-c -Wall -std=c99

all: puny

puny: puny.o
	$(CC) puny.o -o puny

puny.o: puny.c
	$(CC) $(CFLAGS) puny.c

clean:
	rm *o puny
