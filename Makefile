CC=gcc
CFLAGS=-c -Wall
FFLAGS=-lncurses

all: hi

hi:	main.o
	$(CC) $(FFLAGS) main.o -o eigol

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -rf *o hi
