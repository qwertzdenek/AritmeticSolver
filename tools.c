#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void die(const char *msg)
{
  fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}

bool equals(char *compared, char *to)
{
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
