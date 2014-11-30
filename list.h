#ifndef _LIST_H
#define _LIST_H

int count();
int isEmpty();
void push(int *value, char *name);
int pop(int **p_value, char **p_name);
void begin();
int next(int **p_value, char **p_name);
void cleanup();

#endif
