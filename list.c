#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

typedef struct Node_t
{
  char *name;
  int *value;
  struct Node_t *next;
} node_t;

int n = 0;
static struct Node_t *top = NULL;
static struct Node_t *current = NULL;

int count()
{
    return n;
}

bool isEmpty()
{
    return top == NULL;
}

void push(int *value, char *name)
{
    assert(name);
    
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

bool pop(int **p_value, char **p_name)
{
    node_t *tmp;
    
    if (top == NULL)
        return false;
    
    if (p_value != NULL) *p_value = top->value;
    if (p_name != NULL) *p_name = top->name;
    
    tmp = top;
    top = tmp->next;
    n--;

    free(tmp->value);
    free(tmp->name);
    free(tmp);
    
    return true;
}

// nastavý začátek seznamu. Použít před začátkem
// každého průchodu.
void begin()
{
    current = top;
}

bool next(int **p_value, char **p_name)
{
    if (current == NULL)
    {
        current = top;
        if (current == NULL)
            return false;
    }
    
    if (p_value != NULL) *p_value = current->value;
    if (p_name != NULL) *p_name = current->name;
    
    current = current->next;

    if (current == NULL)
      return false;  

    return true;
}

void cleanup()
{
  while (n > 0)
    pop(NULL, NULL);
}

/*
int main(int argc, char *argv[])
{
    int value;
    char *name;
    int foo;
    
    push(5, "a");
    push(12, "b");
    push(10, "d");
    push(8, "g");
    push(16, "tmp");
    push(12, "aaa");
    
    foo = 0;
    while(next(&value, &name))
    {
        printf("next[%d] %s = %d\n", foo, name, value);
        foo++;
    }
    
    pop(&value, &name);
    printf("%s = %d\n", name, value);
    
    while (pop(NULL, NULL))
        ;
    
    return 0;
}
*/
