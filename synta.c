#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lexa.h"
#include "symbols.h"
#include "list.h"

#define ARIT 0
#define BOOL 1
#define UNKN 2

char *nil = "NIL";
char *t = "T";

int list(char *out);
int quote_arg_sym(char *res);

int addii(int a, int b)
{
    return a + b;
}

int subii(int a, int b)
{
    return a - b;
}

int mulii(int a, int b)
{
    return a * b;
}

int divii(int a, int b)
{
    return a / b;
}

int ncomp(int a, int b)
{
    return a != b;
}

int comp(int a, int b)
{
    return a == b;
}

int small(int a, int b)
{
    return a < b;
}

int big(int a, int b)
{
    return a > b;
}

int smalleq(int a, int b)
{
    return a <= b;
}

int bigeq(int a, int b)
{
    return a >= b;
}

int zero(int a, int b)
{
    return 0;
}

int quote_list(char *res)
{
    atom act;
    char tmp[32];
    char *ptr = res;

    *ptr++ = '(';

    lexa_next(&act);

    while (act.type == AT_VAR || act.type == AT_NUM || act.type == AT_LBRACKET || act.type == AT_QUOTE)
    {
        quote_arg_sym(tmp);
        *ptr++ = ' ';
        strcpy(ptr, tmp);
        ptr += strlen(tmp);

        if (lexa_next(&act) == END_CODE)
            break;
    }

    *ptr = ')';

    return OK_CODE;
}

// zpracuje jeden quote argument
int quote_arg_sym(char *res)
{
    atom act;
    lexa_get(&act);

    switch (act.type)
    {
    case AT_NUM:
        sprintf(res, "%d", act.value);
        break;
    case AT_FCE:
    case AT_VAR:
        strcpy(res, act.string);
        break;
    case AT_LBRACKET:
        quote_list(res);
        break;
    case AT_QUOTE:
        *res = '\'';
        lexa_next(NULL);
        quote_arg_sym(res + 1);
        break;
    }

    return OK_CODE;
}

// argument funkce
int arg_sym(char *res)
{
    int *var_v = NULL;
    char *var_n = NULL;
    int found;
    atom act;

    lexa_get(&act);

    switch (act.type)
    {
    case AT_LBRACKET:
        list(res);
        break;
    case AT_VAR:
        // TODO: get variable from the list
        begin();
        found = 0;

        while (next(&var_v, &var_n))
        {
            if (strcmp(var_n, act.string) == 0)
            {
                found = 1;
                break;
            }
        }

        if (found)
            sprintf(res, "%d", *var_v);
        else
        {
            sprintf(error_message, "unknown variable %s\n", act.string);
            return ERROR_CODE;
        }
        break;
    case AT_NUM:
        sprintf(res, "%d", act.value);
        break;
    case AT_RBRACKET:
        return END_CODE;
    }

    return OK_CODE;
}

int arg(char *res)
{
    // if arg_sym or quote_arg_sym
    atom act;
    lexa_get(&act);

    switch (act.type)
    {
    case AT_LBRACKET:
    case AT_NUM:
    case AT_VAR:
        arg_sym(res);
        break;
    case AT_QUOTE:
        lexa_next(&act);
        quote_arg_sym(res);
        break;
    default:
        sprintf(error_message, "syntax error\n");
        return ERROR_CODE;
    }

    return OK_CODE;
}

// uvnitř seznamu
int list_in(char *res)
{
    atom act;
    int (*op)(int, int) = zero;
    int ares = 0;
    int bres;
    int type;
    char val[20];
    int *var_v = NULL;
    char *var_n = NULL;

    lexa_get(&act);

    if (act.type == AT_RBRACKET)
    {
        strcpy(res, nil);
        return OK_CODE;
    }
    else if (act.type != AT_FCE)
    {
        sprintf(error_message, "syntax error\n");
        return ERROR_CODE;
    }

    switch (act.value)
    {
    case ADD:
        op = addii;
        ares = 0;
        type = ARIT;
        break;
    case SUB:
        op = subii;
        type = ARIT;
        lexa_next(&act);
        if (act.type == AT_VAR || act.type == AT_NUM || act.type == AT_LBRACKET)
        {
            arg(val);
            ares = strtol(val, NULL, 10);
        }
        else
        {
            sprintf(error_message, "syntax error\n");
            return ERROR_CODE;
        }
        break;
    case MULT:
        op = mulii;
        ares = 1;
        type = ARIT;
        break;
    case DIV:
        op = divii;
        type = ARIT;
        lexa_next(&act);
        if (act.type == AT_VAR || act.type == AT_NUM || act.type == AT_LBRACKET)
        {
            arg(val);
            ares = strtol(val, NULL, 10);
        }
        else
        {
            sprintf(error_message, "syntax error\n");
            return ERROR_CODE;
        }
        break;
    case QUOTE:
        lexa_next(NULL);
        quote_arg_sym(res);
        lexa_next(NULL);
        return OK_CODE;
    case PRINT:
        lexa_next(NULL);
        arg(res);
        lexa_next(NULL);
        return OK_CODE;
    case SET:
        lexa_next(&act);
        if (act.type == AT_QUOTE)
        {
            if (arg(res) == ERROR_CODE)
                return ERROR_CODE;
            var_n = (char *) malloc(strlen(res) + 1);
            strcpy(var_n, res);

            lexa_next(NULL);
            if (arg(res) == ERROR_CODE)
                return ERROR_CODE;
            var_v = (int *) malloc(sizeof(int));
            *var_v = strtol(res, NULL, 10);
            push(var_v, var_n);
        }
        else
        {
            sprintf(error_message, "set argument error\n");
            return ERROR_CODE;
        }

        lexa_next(NULL);
        return OK_CODE;
    case QUIT:
        lexa_next(NULL);
        return END_CODE;
    default:
        sprintf(error_message, "Not implemented function\n");
        return ERROR_CODE;
    }

    lexa_next(&act);

    while (act.type == AT_VAR || act.type == AT_NUM || act.type == AT_LBRACKET || act.type == AT_QUOTE)
    {
        if (type == ARIT)
        {
            if (arg(val) == ERROR_CODE)
                return ERROR_CODE;
            ares = op(ares, strtol(val, NULL, 10));
        }
        else if (type == BOOL)
        {
            if (arg(val) == ERROR_CODE)
                return ERROR_CODE;
            bres &= op(bres, strtol(val, NULL, 10));
        }

        if (lexa_next(&act) == END_CODE)
            break;
    }

    if (type == ARIT)
    {
        sprintf(res, "%d", ares);
    }
    else
    {
        strcpy(res, bres ? t : nil);
    }

    return OK_CODE;
}

// seznam (ošetřuje závorky)
int list(char *out)
{
    atom act;
    int code;

    lexa_get(&act);
    if (act.type != AT_LBRACKET)
    {
        sprintf(error_message, "This is not a list\n");
        return ERROR_CODE;
    }

    lexa_next(&act);
    code = list_in(out);

    if (code == ERROR_CODE)
        return ERROR_CODE;

    lexa_get(&act);
    if (act.type != AT_RBRACKET)
    {
        sprintf(error_message, "This is not a list\n");
        return ERROR_CODE;
    }

    if (code == END_CODE)
        return END_CODE;
    else
        return OK_CODE;
}

// vyhodnotí seznam nebo squote
int start()
{
    atom act;
    char res[128];
    int code;

    #ifdef __linux__
    lexa_flush();
    #endif // __linux__

    code = lexa_next(&act);
    if (code == END_CODE)
        return END_CODE;
    else if (code == ERROR_CODE)
    {
        sprintf(error_message, "lex error\n");
        return ERROR_CODE;
    }

    if (act.type == AT_QUOTE)
    {
        lexa_next(&act);
        code = quote_arg_sym(res);
    }
    else
        code = list(res);

    if (code == OK_CODE)
        printf("%s\n", res);
    else if (code == END_CODE)
        return END_CODE;
    else if (code == ERROR_CODE)
        return ERROR_CODE;

    return OK_CODE;
}

