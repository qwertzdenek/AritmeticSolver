#ifndef _STACK_H_
#define _STACK_H_

typedef struct {
    int sp; // délka zásobníku
    int size;
    int itemlen;
    void *data;
} stack;

stack *createstack(int size, int itemlen);
void free_stack(stack **s);

int push(stack *s, void *data);
int pop(stack *s, void *data);
int isEmpty(stack *s);

#endif
