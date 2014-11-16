CC=gcc
CFLAGS=-Wall -O2
SOURCES=solver.c lexa.c synta.c
EXECUTABLE=lisp

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

