#include "stack.h"
#include <stdlib.h>
#include <stdbool.h>

stack *createstack(int size, int itemlen)
{
    stack *tmp;
    
    if (size <= 0 || itemlen <= 0) return NULL;
    
    tmp = (stack *) malloc(sizeof(stack));
    if (tmp == NULL) return NULL;
    
    tmp->sp = -1;
    tmp->size = size;
    tmp->itemlen = itemlen;
    tmp->data = malloc(size*itemlen);
    
    if (tmp->data == NULL)
    {
        free(tmp);
        return NULL;
    }
    
    return tmp;
}

int push(stack *s, void *data)
{
    int i;
    
    if (s == NULL || data == NULL) return false;
    
    if (s->sp >= s->size) return false;
    s->sp++;
    
    for (i = 0; i < s->itemlen; i++)
    {
        *((char *) s->data + s->sp * s->itemlen + i) = *((char *) data + i);
    }
    
    return true;
}

int pop(stack *s, void *data)
{
    int i;
    
    if (s == NULL || data == NULL) return false;
    
    if (s->sp < 0) return false;
    
    for (i = 0; i < s->itemlen; i++)
    {
        *((char *) data + i) = *((char *) s->data + s->sp * s->itemlen + i);
    }
    
    s->sp--;
    
    return true;
}

int isEmpty(stack *s)
{
    if (s->sp < 0)
        return true;
    else
        return false;
}

void free_stack(stack **s)
{
    if (s == NULL || *s == NULL) return;
    
    free((*s)->data);
    free(*s);
    *s = NULL;
}

