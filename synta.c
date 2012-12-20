#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexa.h"
#include "tools.h"
#include "list.h"

#define ARIT 1
#define BOOL 0
#define UNKN -1

char *nil = "NIL";
char *t = "T";

void komp(char *res);
void vyraz(char *out);

char *quote(char *res)
{
  atom csym;
  int len = 0;
  
  lexa_get(&csym);
  while (csym.type == AT_VALUE || csym.type == AT_IDENT
	 || csym.type == AT_LBRACKET || csym.type == AT_RBRACKET
	 || csym.type == AT_OPERATOR)
    {
      if (csym.type == AT_LBRACKET)
	{
	  *res = *csym.data;
	  res++;
	  lexa_next(NULL);
	  res = quote(res);
	}
      else if (csym.type == AT_RBRACKET)
	{
	  // TODO: need fix (quote (+ a 22))
	  *res = *csym.data;
     	  res++;
	  break;
	}

      switch (csym.type)
	{
	case AT_OPERATOR:
	  len = strlen(csym.data);
	  memcpy(res, csym.data, len);
	  res += len;
	  break;
	case AT_IDENT:
	  *res++ = ' ';
	  len = strlen(csym.data);
	  memcpy(res, csym.data, len);
	  res += len;
	  break;
	case AT_VALUE:
	  *res++ = ' ';
	  res += sprintf(res, "%d", *((int *) csym.data));
	  break;
	}
      if (!lexa_next(&csym))
	break;
    }

  return res;
}

void squote(char *res)
{
  lexa_next(NULL);
  res = quote(res);
  *res = 0;
}

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

int smalleq(int a, int b)
{
  return a <= b;
}

int bigeq(int a, int b)
{
  return a >= b;
}

void komp(char *res)
{
  atom act;
  int *var_v = NULL;
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
	case (AT_LBRACKET):
	  komp(res);
	  break;
	}
    }
  else if (act.type == AT_IDENT)
    {
      // TODO: Přepsat na konstantní string
      if (equals(act.data, "QUIT"))
	strcpy(res, "QUIT");
      else if (equals(act.data, "SET"))
	{
	  lexa_next(&act);
	  var_n = (char *) malloc(strlen(act.data) + 1);
	  strcpy(var_n, act.data);
	  lexa_next(&act);
	  var_v = (int *) malloc(sizeof(int));
	  *var_v = *((int *) act.data);
	  
	  push(var_v, var_n);
	  sprintf(res, "%d", *var_v);
	}
      else if (equals(act.data, "CAR"))
	{
	  strcpy(res, "CAR");
	}
      else if (equals(act.data, "QUOTE"))
	{
	  strcpy(res, "QUOTE");
	}
      else // is it variable?
	{
	  begin();
	  found = false;
	  for (i = 0; i < count(); i++)
	    {
	      next(&var_v, &var_n);
	      if (equals(var_n, act.data))
		{
		  sprintf(res, "%d", *var_v);
		  found = true;
		  break;
		}
	    }
	  if (!found)
	    printf("(EE) Proměnná \"%s\" nenalezena\n", act.data);
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
      op = subii;
      type = ARIT;
      lexa_next(&act);
      if (act.type == AT_VALUE || act.type == AT_IDENT
	  || act.type == AT_LBRACKET)
	{
	  komp(buf);
	  res = strtol(buf, NULL, 10);
	}
      else
	die("(EE) Nelze vyhodnotit");
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
	  die("(EE) Nelze dělit");
      } else if (act.data[1] == '=') {
	op = ncomp;
	type = BOOL;
      }
      break;
    case '=':
      op = comp;
      type = BOOL;
    case '<':
      if (act.data[1] == 0) {
	op = small;
	type = BOOL;
	lexa_next(&act);
	if (act.type == AT_VALUE || act.type == AT_IDENT
	    || act.type == AT_LBRACKET)
	  {
	    komp(buf);
	    res = strtol(buf, NULL, 10);
	  }
	else
	  die("(EE) Nelze porovnat");
      } else if (act.data[1] == '=') {
	op = smalleq;
	type = BOOL;
      }
      
      break;
    case '>':
      if (act.data[1] == 0) {
	op = big;
	type = BOOL;
	lexa_next(&act);
	if (act.type == AT_VALUE || act.type == AT_IDENT
	    || act.type == AT_LBRACKET)
	  {
	    komp(buf);
	    res = strtol(buf, NULL, 10);
	  }
	else
	  die("(EE) Nelze porovnat");
      } else if (act.data[1] == '=') {
	op = bigeq;
	type = BOOL;
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
	printf("(EE) Neplatný výraz\n");
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
  char res[128];

  // do some initialization
  res[0] = 'Q';
  res[1] = 'U';
  res[2] = 'I';
  res[3] = 'T';
  res[4] = 0;

  lexa_next(&act);
  switch (act.type)
    {
    case AT_OPERATOR:
      vyraz(res);
      printf("%s\n",res);
      break;
     
    default:
      komp(res);
      if (equals(res, "QUIT"))
	{
	  return 0;
	}
      else if (equals(res, "CAR"))
	{
	  lexa_next(&act);
	  komp(res);
	}
      else if (equals(res, "QUOTE"))
	{
	  squote(res);
	}
      printf("%s\n",res);
      break;
    }
  return 1;
}
