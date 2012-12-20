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

#include "lexa.h"
#include "synta.h"
#include "list.h"

#define EPS 1e-6

#define OPSYM_ADD 10
#define OPSYM_SUB 11 
#define OPSYM_MUL 12
#define OPSYM_DIV 13

char *trim(char *ptr)
{
  while(isspace(*ptr++))
    ;

  return ptr;
}

int main(int argc, char *argv[])
{
  char l[100];
  // atom sym;
  int dalsi = 1;
  char *sptr;

  atexit(cleanup);

  do {
    printf(" > ");
    fgets(l, 100, stdin);
    
    sptr = trim(l);
    
    if (*sptr != '0')
      {
	lexa_init(l);

	dalsi = start();
      }
  } while (dalsi > 0);

  /*
  while (lexa_next(&sym))
    {
      printf("type %d\n",sym.type);

      if (sym.type == AT_OPERATOR)
	printf("  %s\n",sym.data);
    }
  */

  return EXIT_SUCCESS;
}

