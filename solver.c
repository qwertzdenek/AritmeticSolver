/*
 * solver.c
 *
 * Copyright 2012 Zdeněk Janeček <jan.zdenek@gmail.com>
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
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "synta.h"
#include "symbols.h"

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

// Pro čtení ze souboru
void readfile(FILE *f, char *l)
{
    int c;
    char *ptr = l;

    c = fgetc(f);

    while (c != 10 && c != 13 && c != EOF)
    {
        *ptr++ = (char) c;
        c = fgetc(f);
    }

    *ptr = 0;
}

int main(int argc, char *argv[])
{
    char l[2048];
    // atom sym;
    int res;
    bool inter = 0;
    FILE *source;
    int counter = 0;

    atexit(cleanup);

    if (argc > 1)
    {
        file = argv[1];
        source = fopen(file, "r");
        if (source == NULL)
        {
            perror(argv[1])
            return EXIT_FAILURE;
        }
    }
    else
    {
        file = NULL;
        inter = 1;
    }

    while (1)
    {
        if (inter)
        {
            printf("[%d]> ",counter);
            fgets(l, 100, stdin);
        }
        else
        {
            // TODO: read whole file
            readfile(source, l);
        }

        lexa_init(l);
        res = start();
        
        counter++;
        
        if (res == OK_CODE)
            continue;
        else if (res == END_CODE)
            break;
        else if (res == ERROR_CODE)
            print_error()
    }

    printf("Bye.\n");

    if (!inter)
        fclose(source);

    return EXIT_SUCCESS;
}

