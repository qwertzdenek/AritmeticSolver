#ifndef _LEX_H_
#define _LEX_H_

#define AT_FCE 1
#define AT_VAR 2
#define AT_NUM 3
#define AT_LBRACKET 4
#define AT_RBRACKET 5
#define AT_UNKNOWN 6

#define FUNS_COUNT 11

extern char error_message[64];

enum ftype
{
    ADD, SUB, MULT, DIV, QUOTE, SET, HELP, QUIT, CAR, CDR, ABOUT
};

typedef struct
{
    int type;
    union
    {
        char string[28];
        int value;
    };
} atom;

void lexa_init(FILE *file);
int lexa_next(atom *sym);
void lexa_get(atom *sym);

#endif

