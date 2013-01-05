CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -O2 -Wno-strict-aliasing
LDFLAGS=-s
SOURCES=solver.c lexa.c synta.c tools.c list.c
EXECUTABLE=lisp.exe

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE)
