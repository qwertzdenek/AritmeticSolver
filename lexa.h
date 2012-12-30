#ifndef _LEX_H_
#define _LEX_H_

#include <stdbool.h>

#define AT_UNKNOWN 0
#define AT_OPERATOR 1
#define AT_IDENT 2
#define AT_VALUE 3
#define AT_LBRACKET 4
#define AT_RBRACKET 5

#define ULTIMATE "What is the meaning of live, the Universe and Everything?\n"

typedef struct {
  unsigned char type;
  char data[32];
} atom;

void lexa_init(char *strin);
bool lexa_next(atom *sym);
void lexa_get(atom *sym);

#endif
