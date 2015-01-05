/*
 * solver.c
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
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "synta.h"
#include "lexa.h"
#include "symbols.h"
#include "list.h"

#define EPS 1e-6

#define OPSYM_ADD 10
#define OPSYM_SUB 11
#define OPSYM_MUL 12
#define OPSYM_DIV 13

char error_message[64];

void print_error()
{
    fprintf(stderr, error_message);
}

// reads whole file
void readfile(FILE *f, char *l)
{
    int c;
    char *ptr = l;

    while ((c = fgetc(f)) != EOF)
    {
        *ptr++ = (char) c;
    }

    *ptr = 0;
}

int main(int argc, char *argv[])
{
    int res;
    int inter = 0;
    FILE *source;
    int counter = 0;
    lexa_state state;

    if (argc > 1)
    {
        source = fopen(argv[1], "r");
        if (source == NULL)
        {
            perror(argv[1]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        source = stdin;
        inter = 1;
    }

    state.stream = source;
    state.lchar = ' '; // initial character
    lexa_init(&state);

    while (1)
    {
        if (inter)
        {
            printf("[%d]> ",counter);
        }

        res = start();

        counter++;

        if (res == OK_CODE)
            continue;
        else if (res == END_CODE)
            break;
        else if (res == ERROR_CODE)
        {
            #ifdef __linux__
                lexa_flush();
            #endif // __linux__
            print_error();
        }
    }

    printf("Bye.\n");

    if (!inter)
        fclose(source);

    // clean variable list
    cleanup();

    return EXIT_SUCCESS;
}

