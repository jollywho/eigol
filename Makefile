CC=gcc
CFLAGS=-c -g -std=c11 -Wall
FFLAGS=-lncurses

all: hi

hi:	main.o
	$(CC) $(FFLAGS) main.o -o eigol

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *o hi
