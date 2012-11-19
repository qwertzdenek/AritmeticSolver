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

#define OPSYM_ADD 10
#define OPSYM_SUB 11 
#define OPSYM_MUL 12
#define OPSYM_DIV 13

typedef struct {
    unsigned char type;
    char data[32];
} atom;

stack *z = NULL;
int acti = 0;

void die(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void cleanup()
{
    free_stack(&z);
}

bool isOperator(char o)
{
    if (o == '+' || o == '*' || o == '-' || o == '/')
        return true;
    else
        return false;
}

char *lex(char *s)
{
    // TODO: ošetřit prázdný nebo triviální vstup.
    atom sym;
    int len = strlen(s);
    int i = 0;
    int j = 0;
    int k = 0;
    while (i < len)
    {
//        printf("  l[%d]=%d\n", i, l[i]);
        if (s[i] == '(')
        {
            j = 1;
            k = i + 1;
            while (j > 0 && k < len)
            {
                if (s[k] == ')') j--;
                if (s[k] == '(') j++;
                printf("k=%d j=%d\n",k,j);
                k++;
            }
            
            if (j == 0)
            {
                sym.type = AT_LIST;
                // přidej do sym.data podřetězec
                strncpy(sym.data, s + i + 1, k - i - 3);
                sym.data[k - i - 2] = 0;
                printf(" res=%s\n",sym.data);
                push(z, (void *) &sym);
            }
            else
            {
                die("chybné ozávorkování");
            }
            j = 0;
            i = k;
        }
        else if ((s[i] >= '0') && (s[i] <= '9'))
        {
            sym.type = AT_VALUE;
            sym.data[j] = s[i];
            j++;
        }
        else if (s[i] == ' ') {
            if (j > 0) {
                sym.data[j] = 0;
                
                j = atoi(sym.data);
                memcpy(sym.data, &j, sizeof(int));
                push(z, (void *) &sym);
                j = 0;
            }
        }
        else
        {
            switch (s[i]) {
                case '+':
                    *sym.data = OPSYM_ADD;
                    break;
                case '-':
                    *sym.data = OPSYM_SUB;
                    break;
                case '*':
                    *sym.data = OPSYM_MUL;
                    break;
                case '/':
                    *sym.data = OPSYM_DIV;
                    break;
                default:
                    i++;
                    continue;
            }
            sym.type = AT_OPERATOR;
            push(z, (void *) &sym);
        }
        
        i++;
    }
    
    if (j > 0)
    {
        sym.data[j] = 0;
                
        j = atoi(sym.data);
        memcpy(sym.data, &j, sizeof(int));
        push(z, (void *) &sym);
        j = 0;
    }
    return;
}

void retez()
{
    
}

char *kompta()
{
    
}

void vyraz(stack *s, atom *opsym, int *rval)
{
    atom sym;
    int res = 0;
    
    if (!pop(s, &sym))
        die("Stack underflow at arr");
    
    if (sym.type == AT_OPERATOR)
    {
        memcpy(opsym, &sym, sizeof(atom));
        *rval = -1;
        return;
    }
    
    arr(s, opsym, &res);
    
    if (res == -1)
    {
        *rval = *((int *)sym.data);
        return;
    }
    
    switch (opsym->data[0])
    {
        case OPSYM_ADD:
            *rval = res + *((int *)sym.data);
            break;
        case OPSYM_SUB:
            *rval = res - *((int *)sym.data);
            break;
        case OPSYM_MUL:
            *rval = res * *((int *)sym.data);
            break;
        case OPSYM_DIV:
            *rval = res / *((int *)sym.data);
            break;
        default:
            die("invalid operator");
    }
}

char *start(char *s)
{
    if (isOperator(*s))
        vyraz();
    else
        kompta();
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

int main(int argc, char *argv[])
{
    char l[50];
    char data[50];
    int res;
    atom opsym;
    
    z = createstack(50, sizeof(atom));
    
    atexit(&cleanup);
    
    printf(" > ");
    fgets(l, 50, stdin);
    
    veta(l);
//    arr(z, &opsym, &res);
    
    pop(z, (void *) data);
    printf(" res=%s\n",data);
    free_stack(&z);
    return 0;
}


