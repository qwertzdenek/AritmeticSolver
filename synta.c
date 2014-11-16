#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lexa.h"
#include "symbols.h"

#define ARIT 0
#define BOOL 1
#define UNKN 2

char *nil = "NIL";
char *t = "T";

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

int arg(char *res)
{
    sprintf(res, "%d", 1);
    return OK_CODE;
}

int list_in(char *res)
{
    atom act;
    int code = END_CODE;
    int (*op)(int, int) = zero;
    int ares;
    int bres;
    int type;
    char val[20];

    if (lexa_next(&act) == ERROR_CODE)
        return ERROR_CODE;

    if (act.type != AT_FCE)
    {
        sprintf(error_message, "syntax error\n");
        return ERROR_CODE;
    }
    else if (act.type == AT_RBRACKET)
    {
        strcpy(res, nil);
        return END_CODE;
    }

    switch (act.value)
    {
    case ADD:
        op = addii;
        ares = 0;
        type = ARIT;
        break;
    default:
        printf("Not implemented\n");
    }

    if (act.value == QUIT)
    {
        lexa_next(&act);
        return END_CODE;
    }

    lexa_next(&act);
    if (act.type == AT_RBRACKET)
    {
        strcpy(res, nil);
        return END_CODE;
    }
    if (act.type == AT_UNKNOWN)
    {
        sprintf(error_message, "unknown symbol error\n");
        return ERROR_CODE;
    }

    while (act.type == AT_VAR || act.type == AT_NUM
            || act.type == AT_LBRACKET)
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

        if (lexa_next(&act) == END_CODE || code == END_CODE)
            return END_CODE;
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

    code = list_in(out);

    lexa_get(&act);
    if (act.type != AT_RBRACKET)
    {
        sprintf(error_message, "This is not a list\n");
        return ERROR_CODE;
    }

    if (code == ERROR_CODE)
        return ERROR_CODE;
    else if (code == END_CODE)
        return END_CODE;

    return OK_CODE;
}

int start()
{
    atom act;
    char res[128];
    int code;

    while (1)
    {
        code = lexa_next(&act);
        if (code == END_CODE)
            return END_CODE;
        else if (code == ERROR_CODE)
        {
            //sprintf(error_message, "lex: %s\n", error_message);
            return ERROR_CODE;
        }

        code = list(res);
        if (code == END_CODE)
            return END_CODE;
        else if (code == ERROR_CODE)
        {
            //sprintf(error_message, "syntax error: %s\n", error_message);
            return ERROR_CODE;
        }
    }

    return OK_CODE;
}

