CC=gcc
CFLAGS=-Wall -pedantic -std=c99
LDFLAGS=-lm
SOURCES=solver.c
EXECUTABLE=solver

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE)
