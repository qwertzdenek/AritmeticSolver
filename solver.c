/*
 * solver.c
 * 
 * Copyright 2012 Zdeněk Janeček <zdenda@lhota4>
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
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "stack.h"

#define EPS 1e-6
#define AT_OPERATOR 0
#define AT_IDENT 1
#define AT_VALUE 2

typedef struct {
    unsigned char type;
    char data[32];
} atom;

char *veta(char *s)
{
    char *tmp[50];
    int pos = 0;
    int i = 0;
    
    while (s[pos])
    {
        switch (s[pos]) {
        case '+':
            printf("plus\n");
            break;
        case '*':
            printf("krát\n");
            break;
        case ' ':
            continue;
        default:
            i = pos;
            while (s[i++] != ' ')
                ;
            memcpy(tmp, s+pos, i - pos);
            printf("našel jsem token: %s\n",*tmp);
            i = pos;
        }
        pos++;
    }
    
    return "nic";
}

double add(double x, double y)
{
  return x + y;
}

double sub(double x, double y)
{
  return x - y;
}

double mul(double x, double y)
{
  return x * y;
}

double dvs(double x, double y)
{
  if (fabs(y) < EPS) {
    printf("Div by 0\n");
    return 0;
  }

  return x / y;
}

double nop(double x, double y)
{
  return 0;
}

bool equals(char *compared, char *to)
{
    int len_compared = strlen(compared);
    int len_to = strlen(compared);
    int i;
    
    if (len_compared != len_to)
        return false;
    
    for (i = 0; i < len_to; i++)
    {
        if (compared[i] != to[i])
            return false;
    }
    return true;
}

void list_atom(char *s, int start, char *p)
{
    int endPos = 0;
    
    while (s[endPos++] != ')')
        ;
    
    memcpy(p, s+start, endPos-2);
    p[endPos-2] = 0;
}

int token_len(char *s, int start)
{
    int i = start;
    while (s[i++] != '\n')
        ;
    
    return i-start;
}

int main(int argc, char *argv[])
{
    char l[50];
    char rl[50];
    int i = 0;
    int j = 0;
    int len = 0;
    int res = 0;
    atom sym;
    stack *z = createstack(50, sizeof(atom));
    
    printf(" > ");
    fgets(l, 50, stdin);
    
    len = strlen(l) - 1;
    // reverze vstupu
    for (i = len - 1; i >= 0; i--)
        rl[j++] = l[i];
    rl[len] = 0;
    
    i = 0;
    j = 0;
    while (i < len)
    {
        printf("  rl[%d]=%d\n", i, rl[i]);
        if ((rl[i] >= '0') && (rl[i] <= '9'))
        {
            sym.type = AT_VALUE;
            sym.data[j] = rl[i];
            j++;
        }
        else if ((rl[i] == '+'))
        {
            sym.type = AT_OPERATOR;
            *sym.data = '+';
            push(z, (void *) &sym);
        }
        else if (rl[i] == ' ') {
            if (j > 0) {
                sym.data[j] = 0;
                push(z, (void *) &sym);
                j = 0;
                printf("push číslo: %s\n", sym.data);
            }
        }
        
        i++;
    }
    
    if (j > 0)
    {
        sym.data[j] = 0;
        push(z, (void *) &sym);
        j = 0;
        printf("push číslo: %s\n", sym.data);
    }
    
//    sym.type = AT_IDENT;
//    strcpy(sym.data, r);
    
    while (!isEmpty(z))
    {
        pop(z, (void *) &sym);
        if (sym.type =! AT_OPERATOR)
            res += atoi(sym.data);
    }
    
    printf(" res=%d\n",res);
    free_stack(&z);
    return 0;
}


