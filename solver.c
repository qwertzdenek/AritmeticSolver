/*
 * solver.c
 * 
 * Copyright 2012 Zdeněk Janeček <zdenda@lhota4>
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
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "list.h"

#define EPS 1e-6

//~ double add(double x, double y)
//~ {
  //~ return x + y;
//~ }
//~ 
//~ double sub(double x, double y)
//~ {
  //~ return x - y;
//~ }
//~ 
//~ double mul(double x, double y)
//~ {
  //~ return x * y;
//~ }
//~ 
//~ double dvs(double x, double y)
//~ {
  //~ if (fabs(y) < EPS) {
    //~ printf("Div by 0\n");
    //~ return 0;
  //~ }
//~ 
  //~ return x / y;
//~ }
//~ 
//~ double nop(double x, double y)
//~ {
  //~ return 0;
//~ }

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
  char l[256];
  
  printf(" > ");
  fgets(l,255,stdin);
  
  if (equals(l, "quit"))
  {
      printf("By anyway!\n");
  }
  
  return 0;
}



