/*
 * list.h
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

#ifndef _LIST_H
#define _LIST_H

#define TYPE_VARIABLE 128
#define TYPE_FUNCTION 129

typedef struct
{
    int par_count;
    char body[256];
} func_t;

typedef struct
{
    int type;
    union
    {
        int value;
        func_t func;
    };
} member_t;

int count();
int isEmpty();
void begin();
void push(member_t *value, char *name);
int pop(member_t **p_value, char **p_name);
int next(member_t **p_value, char **p_name);
void cleanup();

#endif
