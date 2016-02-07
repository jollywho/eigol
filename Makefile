CC=gcc
CFLAGS=-c -g -std=c11 -Wall
FFLAGS=-lncurses

all: eigol

eigol:
	$(CC) $(FFLAGS) main.c -o eigol
