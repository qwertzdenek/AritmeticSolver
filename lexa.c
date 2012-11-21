#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "lexa.h"

static char *ptr = NULL;
static atom csym;

bool is_operator(char o)
{
  if (o == '+' || o == '*' || o == '-' || o == '/')
    return true;
  else
    return false;
}

void lexa_init(char *strin)
{
  assert(strin);
  ptr = strin;
  memset(&csym, 0, sizeof(atom));
}

void lexa_get(atom *sym)
{
  assert(sym);
  memcpy(sym, &csym, sizeof(atom));
}

bool lexa_next(atom *sym)
{
  int ival;
  
  char *tptr;
    
  while (isspace(*ptr))
    ptr++;

  if (*ptr == 0)
    return false;

  if (isdigit(*ptr))
    {
      ival = (int) strtol(ptr, &ptr, 10);
      csym.type = AT_VALUE;
      memcpy(csym.data, &ival, sizeof(int));
    }
  else if (*ptr == '(')
    {
      csym.type = AT_LBRACKET;
      *csym.data = *ptr++;
    }
  else if (*ptr == ')')
    {
      csym.type = AT_RBRACKET;
      *csym.data = *ptr++;
    }
  else if (isalpha(*ptr))
    {
      tptr = ptr;
      while (isalpha(*ptr))
	ptr++;
      csym.type = AT_IDENT;
      memcpy(csym.data, tptr, ptr - tptr + 1);
      *(csym.data + (ptr - tptr + 1)) = 0;
    }
  else if (is_operator(*ptr))
    {
      csym.type = AT_OPERATOR;
      *csym.data = *ptr++;
    }

  if (sym != NULL)
    memcpy(sym, &csym, sizeof(atom));

  return true;
}
