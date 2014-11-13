#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "list.h"
#include "lexa.h"
#include "symbols.h"

char error_message[64];

const char *funs_names[FUNS_COUNT] =
{
    "+", "-", "*", "/", "quote", "set", "help", "quit",
    "car", "cdr", "about"
};

static char *ptr = NULL;
static atom csym;

void print_error()
{
    fprintf(stderr, error_message);
}

int is_operator(char o)
{
    if (o == '+' || o == '*' || o == '-' || o == '/' || o == '=' ||
            o == '<' || o == '>')
        return OK_CODE;
    else
        return END_CODE;
}

// Připravý syntaktický analyzátor na nový řetězec.
void lexa_init(char *strin)
{
    assert(strin);
    ptr = strin;
    memset(&csym, 0, sizeof(atom));
}

// Pouze vrací aktuálně zpracovaný symbol.
void lexa_get(atom *sym)
{
    assert(sym);
    memcpy(sym, &csym, sizeof(atom));
}

void write_atom_fce(atom *sym, funs_types type)
{
    sym->type = AT_FCE;
    sym->value = type;
}

void write_atom_var(atom *sym, char *value)
{
    sym->type = AT_VAR;
    strcpy(sym->string, value);
}

void write_atom_num(atom *sym, int value)
{
    sym->type = AT_NUM;
    sym->value = value;
}

// Přesune se v řetězci na další symbol, zapíše na
// dodanou adresu tělo struktury atom.
int lexa_next(atom *sym)
{
    int ival;
    int i, j;

    char *tptr;

    if (sym == NULL)
    {
        sprintf(error_message, "internal error");
        return ERROR_CODE;
    }

    // přeskočit bílé znaky
    while (isspace(*ptr))
        ptr++;

    // konec řetězce
    if (*ptr == 0)
        return END_CODE;

    if (isdigit(*ptr))
    {
        ival = (int) strtol(ptr, &ptr, 10);
        if (!isspace(*ptr))
        {
            sprintf(error_message, "invalid number");
            return ERROR_CODE;
        }
        write_atom_num(&csym, ival);
    }
    else if (*ptr == '(')
    {
        csym.type = AT_LBRACKET;
        ptr++;
    }
    else if (*ptr == ')')
    {
        csym.type = AT_RBRACKET;
        ptr++;
    }
    else if (isalpha(*ptr) || is_operator(*ptr) || *ptr == '_')
    {
        // is it function?
        for (i = 0; i < FUNS_COUNT; i++)
        {
            tptr = ptr;
            j = 0;
            while (funs_names[i][j] && tolower(*tptr) == funs_names[i][j])
            {
                tptr++;
                j++;
            }

            if (funs_names[i][j] == 0)
            {
                write_atom_fce(&csym, i);
                ptr = tptr;

                memcpy(sym, &csym, sizeof(atom));
                return OK_CODE;
            }
        }

        // unary operator
        if (*ptr == '+' && isdigit(*(ptr+1)))
        {
            ival = (int) strtol(ptr, &ptr, 10);
            if (!isspace(*ptr))
            {
                sprintf(error_message, "invalid number");
                return ERROR_CODE;
            }
            write_atom_num(&csym, ival);
        }
        else if (*ptr == '-' && isdigit(*(ptr+1)))
        {
            ival = (int) strtol(ptr, &ptr, 10);
            if (!isspace(*ptr))
            {
                sprintf(error_message, "invalid number");
                return ERROR_CODE;
            }
            write_atom_num(&csym, -ival);
        }
        else
        {
            while (!isspace(*ptr))
                ptr++;

            // create string from the name
            memcpy(error_message, tptr, ptr - tptr);
            *(error_message + (ptr - tptr)) = 0;
            write_atom_var(&csym, error_message);

            // clear string
            error_message[0] = 0;

            // Převést na Uppercase
            i = 0;
            while (csym.string[i])
            {
                csym.string[i] = toupper(csym.string[i]);
                i++;
            }
        }
    }
    else
    {
        tptr = ptr;
        while (!isspace(*ptr))
            ptr++;

        ival = ptr - tptr;
        memcpy(csym.string, tptr, ival);
        *(csym.string + ival) = 0;
        csym.type = AT_UNKNOWN;
    }

    memcpy(sym, &csym, sizeof(atom));
    return OK_CODE;
}

