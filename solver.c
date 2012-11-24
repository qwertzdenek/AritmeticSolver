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

#include "lexa.h"
#include "synta.h"

#define EPS 1e-6

#define OPSYM_ADD 10
#define OPSYM_SUB 11 
#define OPSYM_MUL 12
#define OPSYM_DIV 13

int main(int argc, char *argv[])
{
  char l[50];
  //  atom sym;
    
  printf(" > ");
  fgets(l, 50, stdin);
    
  lexa_init(l);

  start();

  return EXIT_SUCCESS;
}

