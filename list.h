#ifndef _LIST_H
#define _LIST_H

#define TYPE_VARIABLE 128
#define TYPE_FUNCTION 129

typedef struct
{
    int par_count;
    char body[256];
} func_t;

typedef struct
{
    int type;
    union
    {
        int value;
        func_t func;
    };
} member_t;

int count();
int isEmpty();
void begin();
void push(member_t *value, char *name);
int pop(member_t **p_value, char **p_name);
int next(member_t **p_value, char **p_name);
void cleanup();

#endif
