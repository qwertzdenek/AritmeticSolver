#ifndef _LEX_H_
#define _LEX_H_

#define AT_FCE 1
#define AT_VAR 2
#define AT_NUM 3
#define AT_LBRACKET 4
#define AT_RBRACKET 5
#define AT_QUOTE 6
#define AT_UNKNOWN 7

#define FUNS_COUNT 12

extern char error_message[64];

enum ftype
{
    ADD, SUB, MULT, DIV, QUOTE, SET, HELP, QUIT, CAR, CDR, ABOUT, PRINT
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
void lexa_flush();
const char *get_function_name(int id);

#endif
