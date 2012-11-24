#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexa.h"
#include "tools.h"

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
  
  lexa_get(&act);

  switch (*act.data)
    {
    case '+':
      op = addii;
      res = 0;
      break;
    case '-':
      op = divii;
      res = 0;
      break;
    case '*':
      op = mulii;
      res = 1;
      break;
    case '/':
      op = divii;
      lexa_next(&act);
      if (act.type == AT_VALUE || act.type == AT_IDENT
            || act.type == AT_LBRACKET)
	res = komp();
      else
	die("Nelze dělit");
    }

  lexa_next(&act);
  while (act.type == AT_VALUE || act.type == AT_IDENT
           || act.type == AT_LBRACKET)
    {
      res = op(res, komp());
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
