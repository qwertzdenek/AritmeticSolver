#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexa.h"
#include "tools.h"

#define ARIT 1
#define BOOL 0
#define UNKN -1

int komp();
int vyraz();

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

int komp()
{
  atom act;
  int res;

  lexa_get(&act);

  if (act.type == AT_VALUE)
    {
      memcpy(&res, &act.data, sizeof(int));
    }
  else if (act.type == AT_LBRACKET)
    {
      lexa_next(NULL);
      res = vyraz();
    }
  else
    res = 0;

  return res;
}

int vyraz()
{
  atom act;
  int res;
  int (*op)(int, int);
  int type = UNKN;
  
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
	  res = komp();
	else
	  die("Nelze dělit");
      } else if (act.data[1] == '=') {
	op = ncomp;
	type = BOOL;
	lexa_next(&act);
	if (act.type == AT_VALUE || act.type == AT_IDENT)
	  res = komp();
      }
    }

  lexa_next(&act);
  while (act.type == AT_VALUE || act.type == AT_IDENT
	 || act.type == AT_LBRACKET)
    {
      if (type == ARIT)
	res = op(res, komp());
      else if (type == BOOL)
	res |= op(res, komp());
    if (!lexa_next(&act))
      break;
    }
  return res;
}

void start()
{
  atom act;

  lexa_next(&act);

  switch (act.type)
    {
    case AT_OPERATOR:
      printf("%d\n",vyraz());
      break;
    default:
      printf("%d\n",komp());

    break;
  }
}
