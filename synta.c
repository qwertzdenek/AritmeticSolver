#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexa.h"
#include "tools.h"
#include "list.h"

#define ARIT 1
#define BOOL 0
#define UNKN -1

const char *qe = "QUOTE";
const char *qt = "QUIT";
const char *car = "CAR";
const char *set = "SET";
const char *hp = "HELP";
const char *about = "ABOUT";
const char *list = "LIST";
const char *blank = "";

char *nil = "NIL";
char *t = "T";

void komp(char *res);
void vyraz(char *out);

void quote(char *res)
{
  atom csym;
  int len = 0;
  
  lexa_get(&csym);

  // TODO: vyřešit (quote (+ (- a 2) 3))
  if (csym.type == AT_LBRACKET)
    {
      *res = *csym.data;
      res++;
      lexa_next(NULL);
      quote(res);
    }

  while (csym.type == AT_VALUE || csym.type == AT_IDENT
	 || csym.type == AT_OPERATOR)
    {
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

  if (csym.type == AT_RBRACKET)
    {
      *res = *csym.data;
      res++;
    }
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
      if (equals(act.data, qt))
	strcpy(res, qt);
      else if (equals(act.data, set))
	{
	  lexa_next(&act);
	  var_n = (char *) malloc(strlen(act.data) + 1);
	  strcpy(var_n, act.data);
	  lexa_next(&act);
	  var_v = (int *) malloc(sizeof(int));
	  komp(res);
	  *var_v = strtol(res, NULL, 10);
	  
	  push(var_v, var_n);
	  //	  sprintf(res, "%d", *var_v);
	}
      else if (equals(act.data, car))
	{
	  lexa_next(&act);
	  komp(res);
	  if (equals(res, nil))
	    {
	      printf("Prázdný seznam.");
	      res = '\0';
	    }
	  else
	    {
	    var_n = strchr(res, ';');

	    if (var_n != NULL)
	      *(var_n) = '\0';
	    }
	}
      else if (equals(act.data, qe))
	{
	  strcpy(res, qe);
	}
      else if (equals(act.data, hp) || equals(act.data, about))
	{
	  help();
	  strcpy(res, blank);
	}
      else if (equals(act.data, list))
	{
	  if (lexa_next(&act) && (act.type == AT_VALUE || act.type == AT_IDENT ||
				   act.type == AT_LBRACKET))
	    {
	      var_v = (int *) malloc(sizeof(int));
	      var_n = (char *) malloc(64);
	      *var_v = 0;

	      while (1)
		{
		  komp(res);

		  strcpy(var_n + *var_v, res);
		  *var_v += strlen(res);
	      
		  if (!lexa_next(&act))
		    break;
	      
		  if (act.type == AT_VALUE || act.type == AT_IDENT ||
		      act.type == AT_LBRACKET)
		    {
		      *(var_n + *var_v) = ';';
		      *var_v = *var_v + 1;
		      continue;
		    }
		  else
		    break;
		}
	      *(var_n + *var_v) = '\0';

	      strcpy(res, var_n);
	      free(var_n);
	      free(var_v);
	      var_n = NULL;
	      var_v = NULL;
	    }
	  else
	    strcpy(res, nil);
	}
      else if (equals(act.data, nil))
	strcpy(res, nil);
      else // so is it variable?
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
  /*  else if (act.type = AT_UNKNOWN)
    {
      
    }*/
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

  //  printf("první znak: %d", act.type);

  switch (act.type)
    {
    case AT_OPERATOR:
      vyraz(res);
      printf("%s\n",res);
      break;
     
    default:
      komp(res);
      if (equals(res, qt))
	{
	  return 0;
	}
      else if (equals(res, car))
	{
	  lexa_next(&act);
	  komp(res);
	}
      else if (equals(res, qe))
	{
	  squote(res);
	}
      printf("%s\n",res);
      break;
    }
  return 1;
}
