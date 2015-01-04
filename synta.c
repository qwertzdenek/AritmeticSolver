/*
 * synta.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lexa.h"
#include "symbols.h"
#include "list.h"
#include "tools.h"

#define ARIT 0
#define BOOL 1
#define UNKN 2

char *nil = "NIL";
char *t = "T";

int list(char *out);
int quote_arg_sym(char *res);
int quote_list(char *res);
int call_sub(char *res);
int arg(char *res);
int arg_sym(char *res);
int skip_list();

char *help_text =
    "LISP language interpretter. Written by Zdeněk Janeček in years 2012-2014\n"\
    "Functions:\n"\
    "  aritmetic -> + - * /\n"\
    "  boolean -> == = != <= >= & |\n"\
    "  quote - do not evaluate and print\n"\
    "  set - set variable (set 'a 2)\n"\
    "  if - branch condition\n"\
    "  help - print this message\n"\
    "  quit - exit program\n";

void help(char *res)
{
    strcpy(res, help_text);
}

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

int neq(int a, int b)
{
    return a != b;
}

int eq(int a, int b)
{
    return a == b;
}

int leq(int a, int b)
{
    return a <= b;
}

int geq(int a, int b)
{
    return a >= b;
}

int land(int a, int b)
{
    return a & b;
}

int lor(int a, int b)
{
    return a | b;
}

int zero(int a, int b)
{
    return 0;
}

// return the first element of list
int car_list(char *res)
{
    atom act;
    int type;
    FILE *stream;
    lexa_state state;
    lexa_state state_backup;

    lexa_get(&act);

    type = list(res);
    if (type == END_CODE)
        strcpy(res, nil);
    else if (type == ERROR_CODE)
        return ERROR_CODE;

    stream = fmemopen(res, strlen(res), "r");
    state.stream = stream;
    state.lchar = ' '; // initial character
    state_backup = lexa_init(&state);

    // read first symbol
    lexa_next(&act);
    if (act.type != AT_LBRACKET)
    {
        sprintf(error_message, "car: argument must be a list\n");
        return ERROR_CODE;
    }

    lexa_next(&act);
    if (arg_sym(res) == END_CODE)
        strcpy(res, nil);

    fclose(stream);
    lexa_init(&state_backup);

    return OK_CODE;
}

int cdr_list(char *res)
{
    atom act;
    int type;
    FILE *stream;
    lexa_state state;
    lexa_state state_backup;

    lexa_get(&act);

    type = list(res);
    if (type == END_CODE)
        strcpy(res, nil);
    else if (type == ERROR_CODE)
        return ERROR_CODE;

    stream = fmemopen(res, strlen(res), "r");
    state.stream = stream;
    state.lchar = ' '; // initial character
    state_backup = lexa_init(&state);

    // read first symbol
    lexa_next(&act);
    if (act.type != AT_LBRACKET)
    {
        sprintf(error_message, "cdr: argument must be a list\n");
        return ERROR_CODE;
    }

    lexa_next(&act);
    if (act.type == AT_LBRACKET)
        skip_list();
    if (quote_list(res) == END_CODE)
        strcpy(res, nil);

    fclose(stream);
    lexa_init(&state_backup);

//    lexa_next(NULL);
    return OK_CODE;
}

// přeskočí vnořené seznamy
int skip_list()
{
    atom act;
    int counter = 0;

    lexa_get(&act);

    if (act.type == AT_RBRACKET)
        return END_CODE;
    else if (act.type != AT_LBRACKET)
        return ERROR_CODE;

    while (1)
    {
        switch (act.type)
        {
        case AT_LBRACKET:
            counter++;
            break;
        case AT_RBRACKET:
            counter--;
            break;
        }

        if (counter <= 0)
            break;
        else
            lexa_next(&act);
    }

    return OK_CODE;
}

int next_num(int *var)
{
    atom act;
    char val[16];
    char *tmp;

    lexa_get(&act);

    if (act.type == AT_VAR || act.type == AT_NUM || act.type == AT_LBRACKET)
    {
        if (arg(val) == ERROR_CODE)
            return ERROR_CODE;
        if (strcmp(val, t) == 0)
            *var = 1;
        else if (strcmp(val, nil) == 0)
            *var = 0;
        else
        {
            *var = strtol(val, &tmp, 10);
            if ((char *) val == tmp)
            {
                sprintf(error_message, "this %s is not a valid operand\n", val);
                return ERROR_CODE;
            }
        }
    }
    else
    {
        sprintf(error_message, "syntax error\n");
        return ERROR_CODE;
    }

    return OK_CODE;
}

int get_var(char *name, member_t *res, char **stored_name)
{
    member_t *var_v;
    char *var_n = NULL;
    int found = 0;

    begin();

    while (next(&var_v, &var_n))
    {
        if (strcmp(var_n, name) == 0)
        {
            if (stored_name != NULL) *stored_name = var_n;
            found = 1;
            break;
        }
    }

    if (found)
        memcpy(res, var_v, sizeof(member_t));
    else
    {
        sprintf(error_message, "unknown variable %s\n", name);
        return ERROR_CODE;
    }

    return OK_CODE;
}

int get_num_list(int **res, int *cres)
{
    atom act;
    int i;
    char tmp[64];
    member_t *mem;
    FILE *stream;
    lexa_state state;
    lexa_state state_backup;

    *cres = 0;

    if (arg(tmp) == ERROR_CODE)
        return ERROR_CODE;

    stream = fmemopen(tmp, strlen(tmp), "r");
    state.stream = stream;
    state.lchar = ' '; // initial character
    state_backup = lexa_init(&state);

    lexa_next(&act);

    if (act.type == AT_LBRACKET)
        lexa_next(&act);

    while (act.type == AT_VAR || act.type == AT_NUM)
    {
        *cres = *cres + 1;
        arg_sym(tmp);

        // parse number
        mem = (member_t *) malloc(sizeof(member_t));
        mem->value = (int) strtol((char *) tmp, NULL, 10);
        push(mem, NULL);
        if (lexa_next(&act) == END_CODE)
            break;
    }

    // load variables to res
    *res = (int *) malloc(*cres * sizeof(int));
    for (i = 0; i < *cres; i++)
    {
        pop(&mem, NULL);
        (*res)[i] = mem->value;
        free(mem);
    }

    fclose(stream);
    lexa_init(&state_backup);

    return OK_CODE;
}

int quote_list(char *res)
{
    atom act;
    char tmp[32];
    char *ptr = res;
    int first = 1;

    *ptr++ = '(';

    lexa_next(&act);

    while (act.type == AT_FCE || act.type == AT_VAR || act.type == AT_NUM || act.type == AT_LBRACKET || act.type == AT_QUOTE)
    {
        quote_arg_sym(tmp);
        if (first) first = 0;
        else *ptr++ = ' ';
        strcpy(ptr, tmp);
        ptr += strlen(tmp);

        if (lexa_next(&act) == END_CODE)
            break;
    }

    *ptr++ = ')';
    *ptr = 0;

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
        strcpy(res, get_function_name(act.value));
        break;
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
    member_t mem;
    atom act;

    lexa_get(&act);

    switch (act.type)
    {
    case AT_LBRACKET:
        list(res);
        break;
    case AT_VAR:
        if (get_var(act.string, &mem, NULL) == ERROR_CODE)
            return ERROR_CODE;
        if (mem.type == TYPE_FUNCTION)
        {
            sprintf(error_message, "syntax error (function argument)\n");
            return ERROR_CODE;
        }
        sprintf(res, "%d", mem.value);
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
    int boolres;
    int type;

    member_t mem;
    func_t func;
    char *var_n = NULL;
    member_t *var_v;
    int *args;   // function args array
    int cargs = 0;   // args count
    FILE *stream;
    lexa_state state;
    lexa_state state_backup;

    lexa_get(&act);

    if (act.type == AT_RBRACKET)
    {
        strcpy(res, nil);
        return OK_CODE;
    }
    else if (act.type == AT_VAR
             && get_var(act.string, &mem, &var_n) != ERROR_CODE
             && mem.type == TYPE_FUNCTION)
    {
        if (mem.func.par_count > 0)
        {
            lexa_next(&act);
            type = get_num_list(&args, &cargs);
        }

        if (cargs != mem.func.par_count)
        {
            sprintf(error_message, "got %d arguments but %s function expects %d\n", cargs, var_n, mem.func.par_count);
            return ERROR_CODE;
        }

        var_n = (char *) malloc(1024); // function will not be longer than kB, will be?
        replnph(var_n, mem.func.body, args, cargs);
        free(args);

        stream = fmemopen(var_n, strlen(var_n), "r");
        state.stream = stream;
        state.lchar = ' '; // initial character
        state_backup = lexa_init(&state);

        // call function
        lexa_next(NULL);
        type = list(res);
        fclose(stream);
        lexa_init(&state_backup);
        free(var_n);
        lexa_next(NULL);
        return type;
    }
    else if (act.type == AT_FCE)
    {
        // the rest of this function
    }
    else
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
        if (next_num(&ares) == ERROR_CODE)
            return ERROR_CODE;
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
        if (next_num(&ares) == ERROR_CODE)
            return ERROR_CODE;
        break;
    case EQ:
    case EQQ:
        op = eq;
        type = BOOL;
        boolres = 1;
        lexa_next(&act);
        if (next_num(&ares) == ERROR_CODE)
            return ERROR_CODE;
        break;
    case NEQ:
        op = neq;
        type = BOOL;
        boolres = 1;
        lexa_next(&act);
        if (next_num(&ares) == ERROR_CODE)
            return ERROR_CODE;
        break;
    case LEQ:
        op = leq;
        type = BOOL;
        boolres = 1;
        lexa_next(&act);
        if (next_num(&ares) == ERROR_CODE)
            return ERROR_CODE;
        break;
    case GEQ:
        op = geq;
        type = BOOL;
        boolres = 1;
        lexa_next(&act);
        if (next_num(&ares) == ERROR_CODE)
            return ERROR_CODE;
        break;
    case LAND:
        op = land;
        type = BOOL;
        boolres = 1;
        lexa_next(&act);
        if (next_num(&ares) == ERROR_CODE)
            return ERROR_CODE;
        break;
    case LOR:
        op = lor;
        type = BOOL;
        boolres = 1;
        lexa_next(&act);
        if (next_num(&ares) == ERROR_CODE)
            return ERROR_CODE;
        break;
    case LIST:
        quote_list(res);
        return OK_CODE;
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
    case CAR:
        lexa_next(NULL);
        car_list(res);
        lexa_next(NULL);
        return OK_CODE;
    case CDR:
        lexa_next(NULL);
        cdr_list(res);
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
            var_v = (member_t *) malloc(sizeof(member_t));
            var_v->value = strtol(res, NULL, 10);
            var_v->type = TYPE_VARIABLE;
            push(var_v, var_n);
        }
        else
        {
            sprintf(error_message, "set argument error\n");
            return ERROR_CODE;
        }

        lexa_next(NULL);
        return OK_CODE;
    case DEFUN:
        lexa_next(&act);
        if (act.type != AT_VAR)
        {
            sprintf(error_message, "defun syntax error\n");
            return ERROR_CODE;
        }

        var_n = (char *) malloc(strlen(act.string) + 1);
        strcpy(var_n, act.string);

        lexa_next(&act);
        if (act.type != AT_NUM)
        {
            sprintf(error_message, "defun syntax error\n");
            return ERROR_CODE;
        }

        func.par_count = act.value;

        lexa_next(&act);
        quote_arg_sym(res);
        lexa_next(NULL);

        strcpy(func.body, res);
        var_v = (member_t *) malloc(sizeof(member_t));
        var_v->func = func;
        var_v->type = TYPE_FUNCTION;
        push(var_v, var_n);

        return OK_CODE;
    case BIF:
        lexa_next(&act); // read condition
        if (next_num(&ares) == ERROR_CODE)
            return ERROR_CODE;
        lexa_next(&act);
        if (ares) // if true condition
        {
            if (list(res) == ERROR_CODE) // read true branch
            {
                sprintf(error_message, "if syntax error\n");
                return ERROR_CODE;
            }
            lexa_next(&act);
            type = skip_list();
            if (type == ERROR_CODE) // skip false branch
            {
                sprintf(error_message, "if syntax error\n");
                return ERROR_CODE;
            }
            else if (type == OK_CODE)
                lexa_next(NULL);
        }
        else // if false condition
        {
            if (skip_list() == ERROR_CODE) // skip true branch
            {
                sprintf(error_message, "if syntax error\n");
                return ERROR_CODE;
            }
            lexa_next(&act);
            type = list(res);
            if (type == OK_CODE) // read false branch
                lexa_next(NULL);
            else
                strcpy(res, nil);     // false condition was empty
        }
        return OK_CODE;
    case QUIT:
        lexa_next(NULL);
        return END_CODE;
    case HELP:
        help(res);
        lexa_next(NULL);
        return OK_CODE;
    default:
        sprintf(error_message, "Not implemented function\n");
        return ERROR_CODE;
    }

    lexa_next(&act);

    while (act.type == AT_VAR || act.type == AT_NUM || act.type == AT_LBRACKET)
    {
        if (type == ARIT)
        {
            if (next_num(&bres) == ERROR_CODE)
                return ERROR_CODE;
            ares = op(ares, bres);
        }
        else if (type == BOOL)
        {
            if (next_num(&bres) == ERROR_CODE)
                return ERROR_CODE;
            boolres &= op(ares, bres);
            ares = bres;
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
        strcpy(res, boolres ? t : nil);
    }

    return OK_CODE;
}

// list (checks for brackets)
int list(char *out)
{
    atom act;
    int code;
    char tmp[128];

    if (out == NULL)
        out = (char *) tmp;

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
    char res[1024];
    int code;

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

