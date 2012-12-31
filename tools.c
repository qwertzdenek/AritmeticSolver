#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

char *strip(char *s)
{
  char *end = s + strlen(s) - 1; 
  
  while (isspace((int) *s))
    s++;
  
  while (end > s && isspace((int) *end))
    end--;
  *(end + 1) = 0;
  
  return s;
}

void help()
{
  printf("LISP style solver by Zdeněk Janeček 2012\n");
  printf("This program comes with ABSOLUTELY NO WARRANTY!\n");
  printf("This is free software, and you are welcome to redistribute it under conditions of the GNU GPL license version 3.\n");
}

void die(const char *msg)
{
  fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}

bool equals(char *compared, const char *to, int stripit)
{
  if (stripit)
    compared = strip(compared);
  
  int len_compared = strlen(compared);
  int len_to = strlen(to);
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
