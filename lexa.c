#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#ifdef __linux__
#include <stdio_ext.h>
#endif // __linux__

#include "lexa.h"
#include "symbols.h"
#include "tools.h"

const char *funs_names[FUNS_COUNT] =
{
    "+", "-", "*", "/", "QUOTE", "SET", "HELP", "QUIT",
    "CAR", "CDR", "ABOUT", "PRINT", "DEFUN"
};

char lchar;
FILE *file = NULL;
static atom csym;

int is_operator(char o)
{
    if (o == '+' || o == '*' || o == '-' || o == '/' || o == '=' ||
            o == '<' || o == '>')
        return OK_CODE;
    else
        return END_CODE;
}

// sets new stream and return old
lexa_state lexa_init(lexa_state *s)
{
    lexa_state state;

    state.stream = file;
    state.lchar = lchar;
    state.csym = csym;

    if (s != NULL) file = s->stream;
    if (s != NULL) lchar = s->lchar;
    if (s != NULL) csym = s->csym;

    return state;
}

#ifdef __linux__
void lexa_flush()
{
    lchar = ' ';
    __fpurge(file);
}
#endif // __linux__

const char *get_function_name(int id)
{
    return funs_names[id];
}

// Pouze vrací aktuálně zpracovaný symbol.
void lexa_get(atom *sym)
{
    assert(sym);
    memcpy(sym, &csym, sizeof(atom));
}

void write_atom_fce(atom *sym, enum ftype type)
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

void write_atom_quote(atom *sym)
{
    sym->type = AT_QUOTE;
}

// Přesune se v řetězci na další symbol, zapíše na
// dodanou adresu tělo struktury atom.
int lexa_next(atom *sym)
{
    int ival;
    int i, j;
    char buf[32];
    char *ptr;

    // přeskočit bílé znaky
    while (isspace(lchar))
        lchar = fgetc(file);

    // konec řetězce
    if (lchar == EOF)
        return END_CODE;

    if (isdigit(lchar))
    {
        // we already have one character
        buf[0] = lchar;
        ptr = (char *) (buf + 1);
        while (isdigit(lchar = fgetc(file)))
            *ptr++ = lchar;
        *ptr = 0;

        if (isspace(lchar) && (lchar == '(') && (lchar == ')'))
        {
            sprintf(error_message, "invalid number\n");
            return ERROR_CODE;
        }
        ival = (int) strtol((char *) buf, NULL, 10);

        write_atom_num(&csym, ival);
    }
    else if (lchar == '(')
    {
        csym.type = AT_LBRACKET;
        lchar = fgetc(file);
    }
    else if (lchar == ')')
    {
        csym.type = AT_RBRACKET;
        lchar = fgetc(file);
    }
    else if (isalpha(lchar) || is_operator(lchar) || lchar == '_' || lchar == '#')
    {
        buf[0] = lchar;
        ptr = (char *) (buf + 1);
        while (isalpha(lchar = fgetc(file)) || is_operator(lchar) || isdigit(lchar) || lchar == '_' || lchar == '#')
            *ptr++ = lchar;
        *ptr = 0;

        // unary operator
        if ((buf[0] == '+' || buf[0] == '-') && isdigit(buf[1]))
        {
            ival = (int) strtol((char *) buf, &ptr, 10);
            if (*ptr != 0)
            {
                sprintf(error_message, "invalid number\n");
                return ERROR_CODE;
            }
            write_atom_num(&csym, ival);

            if (sym != NULL)
                memcpy(sym, &csym, sizeof(atom));
            return OK_CODE;
        }

        // is it function?
        for (i = 0; i < FUNS_COUNT; i++)
        {
            ptr = (char *) buf;
            j = 0;
            while (funs_names[i][j] && toupper(*ptr) == funs_names[i][j])
            {
                ptr++;
                j++;
            }

            if (*ptr == 0 && funs_names[i][j] == 0)
            {
                write_atom_fce(&csym, i);

                if (sym != NULL)
                    memcpy(sym, &csym, sizeof(atom));
                return OK_CODE;
            }
        }

        // create string from the name
        write_atom_var(&csym, supper(buf));
    }
    else if (lchar == '\'')
    {
        write_atom_quote(&csym);
        lchar = fgetc(file);
    }
    else
    {
        buf[0] = lchar;
        ptr = (char *) (buf + 1);

        while (!isspace(lchar = fgetc(file)) && lchar != '(' && lchar != ')')
            *ptr++ = lchar;
        *ptr = 0;

        strcpy(csym.string, buf);
        csym.type = AT_UNKNOWN;
    }

    if (sym != NULL)
        memcpy(sym, &csym, sizeof(atom));
    return OK_CODE;
}

