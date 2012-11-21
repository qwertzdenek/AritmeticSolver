CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -g
LDFLAGS=-lm
SOURCES=solver.c lexa.c
EXECUTABLE=solver

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE)
