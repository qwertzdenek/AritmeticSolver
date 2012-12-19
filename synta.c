#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexa.h"
#include "tools.h"
#include "list.h"

#define ARIT 1
#define BOOL 0
#define UNKN -1

char *quit = "32766";
char *nil = "NIL";
char *t = "T";

void komp(char *res);
void vyraz(char *out);

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

int ncomp(int a, int b)
{
  return a != b;
}

int comp(int a, int b)
{
  return a == b;
}

int small(int a, int b)
{
  return a < b;
}

int big(int a, int b)
{
  return a > b;
}

void komp(char *res)
{
  atom act;
  int var_v;
  char *var_n = NULL;
  int i;
  bool found;

  lexa_get(&act);

  if (act.type == AT_VALUE)
    {
      sprintf(res, "%d", *((int *) act.data));
    } 
  else if (act.type == AT_LBRACKET)
    {
      lexa_next(&act);
      switch (act.type)
	{
	case (AT_OPERATOR):
	  vyraz(res);
	  break;

	case (AT_VALUE):
	case (AT_IDENT):
	  komp(res);
	  break;
	}
    }
  else if (act.type == AT_IDENT)
    {
      if (equals(act.data, "QUIT"))
	res = quit;
      else
	{
	  begin();
	  found = false;
	  for (i = 0; i < count(); i++)
	    {
	      next(&var_v, &var_n);
	      if (equals(var_n, act.data))
		{
		  sprintf(res, "%d", var_v);
		  found = true;
		  break;
		}
	    }
	  if (!found)
	    die("Proměnná nenalezena");
	}
    }
}

void vyraz(char *out)
{
  atom act;
  int res;
  bool bres = true;
  int (*op)(int, int);
  int type = UNKN;
  char buf[20];
  
  lexa_get(&act);

  switch (*act.data)
    {
    case '+':
      op = addii;
      res = 0;
      type = ARIT;
      break;
    case '-':
      op = divii;
      type = ARIT;
      res = 0;
      break;
    case '*':
      op = mulii;
      type = ARIT;
      res = 1;
      break;
    case '/':
      if (act.data[1] == 0) {
	op = divii;
	type = ARIT;
	lexa_next(&act);
	if (act.type == AT_VALUE || act.type == AT_IDENT
            || act.type == AT_LBRACKET)
	  {
	    komp(buf);
	    res = strtol(buf, NULL, 10);
	  }
	else
	  die("Nelze dělit");
      } else if (act.data[1] == '=') {
	op = ncomp;
	type = BOOL;
	break;
      case '=':
	op = comp;
	type = BOOL;
      case '<':
	op = small;
	type = BOOL;
	break;
      case '>':
	op = big;
	type = BOOL;
	break;
      }
    }

  // inicializace
  if (type == BOOL)
    {
      lexa_next(&act);
      if (act.type == AT_VALUE || act.type == AT_IDENT)
	{
	  komp(buf);
	  res = strtol(buf, NULL, 10);
	}
      else
	die("Neplatný výraz");
    }

  lexa_next(&act);
  while (act.type == AT_VALUE || act.type == AT_IDENT
	 || act.type == AT_LBRACKET)
    {
      if (type == ARIT)
	{
	  komp(buf);
	  res = op(res, strtol(buf, NULL, 10));
	}
      else if (type == BOOL)
	{
	  komp(buf);
	  bres &= op(res, strtol(buf, NULL, 10));
	}
      if (!lexa_next(&act))
	break;
    }

  if (type == ARIT)
    {
      sprintf(out, "%d", res);
    }
  else
    {
      strcpy(out, bres ? t : nil);
    }
}

int start()
{
  atom act;
  char res[20];
  
  push(2, "A");
  push(5, "B");
  push(7, "C");
  push(8, "D");
  push(10, "BETA");

  lexa_next(&act);
  switch (act.type)
    {
    case AT_OPERATOR:
      vyraz(res);
      printf("%s\n",res);
      break;
     
    default:
      komp(res);
      if (equals(res, quit))
	{
	  return 0;
	}
      printf("%s\n",res);
      break;
    }
  return 1;
}
