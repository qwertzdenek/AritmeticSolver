CC=gcc
CFLAGS=-Wall -pedantic -ansi
LDFLAGS=-lm
SOURCES=solver.c
EXECUTABLE=solver

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE)