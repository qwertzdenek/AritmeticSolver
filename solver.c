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

#include "lexa.h"
#include "synta.h"
#include "list.h"
#include "tools.h"

#define EPS 1e-6

#define OPSYM_ADD 10
#define OPSYM_SUB 11 
#define OPSYM_MUL 12
#define OPSYM_DIV 13

void readl(FILE *f, char *l)
{
  int c;
  char *ptr = l;

  c = fgetc(f);

  while (c != 10 && c != 13)
    {
      *ptr++ = (char) c;
      c = fgetc(f);
    }
  
  *ptr = 0;
}


bool has_next_line(FILE *f)
{
  int c;
  bool res;

  c = fgetc(f);

  if ((c != 10 && c != 13) && c != EOF)
    res = true;
  else
    res = false;
  
  fseek(f, -1, SEEK_CUR);
  return res;
}

int main(int argc, char *argv[])
{
  char l[100];
  // atom sym;
  int dalsi = 1;
  char *file;
  bool inter = 0;
  FILE *source;

  if (argc > 1)
    {
      file = argv[1];
      errno = 0;
      source = fopen(file, "r");
      if (source == NULL)
	{
	  printf("Error \"%s\" opening file!\n", strerror(errno));
	  return EXIT_FAILURE;
	}
    }
  else
    {
      file = NULL;
      inter = 1;
    }

  atexit(cleanup);

  do {
    if (inter)
      {
	printf(" > ");
	fgets(l, 100, stdin);
      }
    else
      {
	readl(source, l);
      }
    
    if (equals(l, ULTIMATE))
      {
	printf("42\n");
	dalsi = 1;
	continue;
      }

    if (*(l+1) != 0)
      {
	lexa_init(l);

	dalsi = start();
      }

    if (!inter)
      {
	dalsi = has_next_line(source);
      }
  } while (dalsi > 0);
  
  if (!inter)
    fclose(source);

  return EXIT_SUCCESS;

}

