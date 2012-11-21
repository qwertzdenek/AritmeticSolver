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
#include <stdbool.h>
#include <string.h>

#include "lexa.h"

#define EPS 1e-6

#define OPSYM_ADD 10
#define OPSYM_SUB 11 
#define OPSYM_MUL 12
#define OPSYM_DIV 13

void die(const char *msg)
{
  fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}

/*
void vyraz(stack *s, atom *opsym, int *rval)
{
  atom sym;
  int res = 0;
    
  if (!pop(s, &sym))
    die("Stack underflow at arr");
    
  if (sym.type == AT_OPERATOR)
    {
      memcpy(opsym, &sym, sizeof(atom));
      *rval = -1;
      return;
    }
    
  vyraz(s, opsym, &res);   
  if (res == -1)
    {
      *rval = *((int *)sym.data);
      return;
    }
    
  switch (opsym->data[0])
    {
    case OPSYM_ADD:
      *rval = res + *((int *)sym.data);
      break;
    case OPSYM_SUB:
      *rval = res - *((int *)sym.data);
      break;
    case OPSYM_MUL:
      *rval = res * *((int *)sym.data);
      break;
    case OPSYM_DIV:
      *rval = res / *((int *)sym.data);
      break;
    default:
      die("invalid operator");
    }
}
*/

bool equals(char *compared, char *to)
{
  int len_compared = strlen(compared);
  int len_to = strlen(compared);
  int i;
    
  if (len_compared != len_to)
    return false;
    
  for (i = 0; i < len_to; i++)
    {
      if (compared[i] != to[i])
	return false;
    }
  return true;
}

int main(int argc, char *argv[])
{
  char l[50];
  atom sym;
    
  printf(" > ");
  fgets(l, 50, stdin);
    
  lexa_init(l);
  while (lexa_next(&sym))
    {
      switch (sym.type)
	{
	case AT_VALUE:
	  printf("číslo %d\n", *((int *) sym.data));
	  break;
	case AT_IDENT:
	  printf("ident %s\n", sym.data);
	  break;
	case AT_OPERATOR:
	  printf("operator %c\n", (char) sym.data[0]);
	  break;
	default:
	  printf("nevím\n");
	  break;
	}
    }
  return EXIT_SUCCESS;
}

