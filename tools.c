#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

void die(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

char *supper(char *upper_str)
{
    char *ch = upper_str;

    if (upper_str == NULL)
        return NULL;

    while (*ch)
    {
        *ch = toupper(*ch);
        ch++;
    }
    return upper_str;
}
