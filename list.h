#ifndef _LIST_H
#define _LIST_H

#include <stdbool.h>

int count();
bool isEmpty();
void push(int *value, char *name);
bool pop(int **p_value, char **p_name);
void begin();
bool next(int **p_value, char **p_name);
void cleanup();

#endif
