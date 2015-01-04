/*
 * lexa.h
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

#ifndef _LEX_H_
#define _LEX_H_

#define AT_FCE 1
#define AT_VAR 2
#define AT_NUM 3
#define AT_LBRACKET 4
#define AT_RBRACKET 5
#define AT_QUOTE 6
#define AT_UNKNOWN 7

#define FUNS_COUNT 22

enum ftype
{
    ADD, SUB, MULT, DIV, QUOTE, SET, HELP, QUIT, CAR, CDR,
    ABOUT, PRINT, DEFUN, EQ, EQQ, NEQ, GEQ, LEQ, LAND, LOR, BIF, LIST
};

typedef struct
{
    int type;
    union
    {
        char string[28];
        int value;
    };
} atom;

typedef struct
{
    char lchar;
    FILE *stream;
    atom csym;
} lexa_state;

lexa_state lexa_init(lexa_state *s);
int lexa_next(atom *sym);
void lexa_get(atom *sym);
void lexa_flush();
const char *get_function_name(int id);

#endif
