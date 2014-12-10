#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "symbols.h"

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

int replnph(char *dest, char *src, int *args, int cargs)
{
    char *srcptr = src;
    char *destptr = dest;
    char number[16]; // big enough ;)
    int value;
    int len;

    while (*srcptr != 0)
    {
        if (*srcptr != '#')
        {
            *destptr = *srcptr;
            srcptr++;
            destptr++;
        }
        else
        {
            srcptr++; // skip #
            value = (int) strtol(srcptr, &srcptr, 10); // read value
            if (value < cargs)
                sprintf(number, "%d", args[value]);
            else
            {
//                sprintf(error_message, "invalid variable placeholder %d\n", value);
                return ERROR_CODE;
            }
            len = strlen(number); // string length
            memcpy(destptr, number, len);
            destptr += len;
        }
    }
    *destptr = 0;
    return OK_CODE;
}
