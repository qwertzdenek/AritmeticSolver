#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

typedef struct Node_t
{
    char *name;
    member_t *value;
    struct Node_t *next;
} node_t;

int n = 0;
static struct Node_t *top = NULL;
static struct Node_t *current = NULL;

int count()
{
    return n;
}

int isEmpty()
{
    return top == NULL;
}

void push(member_t *value, char *name)
{
    node_t *node = (node_t *) malloc(sizeof(node_t));
    if (node == NULL)
    {
        perror("Malloc failed");
        return;
    }

    node->value = value;
    node->name = name;

    if (top == NULL)
        node->next = NULL;
    else
        node->next = top;

    top = node;
    n++;
}

int pop(member_t **p_value, char **p_name)
{
    node_t *tmp;

    if (top == NULL)
        return 0;

    if (p_value != NULL) *p_value = top->value;
    if (p_name != NULL) *p_name = top->name;

    tmp = top;
    top = tmp->next;
    n--;

    free(tmp);

    return 1;
}

// nastavý začátek seznamu. Použít před začátkem
// každého průchodu.
void begin()
{
    current = top;
}

int next(member_t **p_value, char **p_name)
{
    if (current == NULL)
        return 0;

    if (p_value != NULL) *p_value = current->value;
    if (p_name != NULL) *p_name = current->name;

    current = current->next;

    return 1;
}

void cleanup()
{
    member_t *p_value;
    char *p_name;

    while (n > 0)
    {
        pop(&p_value, &p_name);
        free(p_value);
        free(p_name);
    }
}
