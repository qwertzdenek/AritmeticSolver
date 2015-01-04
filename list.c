/*
 * list.c
 *
 * Copyright 2012-2014 Zdeněk Janeček <jan.zdenek@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

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
