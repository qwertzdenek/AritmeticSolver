CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -g
LDFLAGS=
SOURCES=solver.c lexa.c synta.c tools.c
EXECUTABLE=solver

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE)
