CC=gcc
CFLAGS=-Wall -O2
SOURCES=solver.c lexa.c synta.c list.c tools.c
EXECUTABLE=lisp

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

clean:
	rm lisp

