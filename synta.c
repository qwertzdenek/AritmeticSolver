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

void quote_arg(char *res)
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
//    case AT_LBRACKET:
//      lexa_next(&act);
//      quote_list(res);
//      break;
    }
}

int quote(char *res)
{
    atom act;

    lexa_get(&act);
    quote_arg(res);

    return OK_CODE;
}

int arg(char *res)
{
    atom act;

    lexa_get(&act);

    switch (act.type)
    {
    case AT_LBRACKET:
        list(res);
        break;
    case AT_VAR:
        strcpy(res, act.string);
        break;
    case AT_NUM:
        sprintf(res, "%d", act.value);
        break;
    case AT_RBRACKET:
        return END_CODE;
    }

    return OK_CODE;
}

int list_in(char *res)
{
    atom act;
    int code = END_CODE;
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
        lexa_next(&act);

        quote(res);
        break;
    case SET:
        lexa_next(&act);
        if (act.type == AT_FCE && *act.string == '\'')
        {
            arg(res);
            var_n = (char *) malloc(strlen(res) + 1);
            strcpy(var_n, res);

            lexa_next(&act);
            arg(res);
            var_v = (int *) malloc(sizeof(int));
            *var_v = strtol(res, NULL, 10);
            push(var_v, var_n);
        }
        else
        {
            sprintf(error_message, "set argument error\n");
            return ERROR_CODE;
        }

        lexa_next(NULL)
        return OK_CODE;
    case QUIT:
        lexa_next(&act);
        return END_CODE;
    default:
        sprintf(error_message, "Not implemented\n");
        return ERROR_CODE;
    }

    while (lexa_next(&act) && (act.type == AT_VAR || act.type == AT_NUM
                               || act.type == AT_LBRACKET))
    {
        if (type == ARIT)
        {
            code = arg(val);
            ares = op(ares, strtol(val, NULL, 10));
        }
        else if (type == BOOL)
        {
            code = arg(val);
            bres &= op(bres, strtol(val, NULL, 10));
        }

        if (code == END_CODE)
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

int start()
{
    atom act;
    char res[128];
    int code;

    code = lexa_next(&act);
    if (code == END_CODE)
        return END_CODE;
    else if (code == ERROR_CODE)
    {
        //sprintf(error_message, "lex: %s\n", error_message);
        return ERROR_CODE;
    }

    code = list(res);
    if (code == OK_CODE)
        printf("%s\n", res);
    else if (code == END_CODE)
        return END_CODE;
    else if (code == ERROR_CODE)
    {
        //sprintf(error_message, "syntax error: %s\n", error_message);
        return ERROR_CODE;
    }

    return OK_CODE;
}

