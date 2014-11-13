#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lexa.h"
#include "tools.h"
#include "list.h"
#include "symbols.h"

#define ARIT 1
#define BOOL 0
#define UNKN -1

const char *qe = "QUOTE";
const char *qt = "QUIT";
const char *car = "CAR";
const char *cdr = "CDR";
const char *set = "SET";
const char *hp = "HELP";
const char *about = "ABOUT";
const char *list = "LIST";
const char *blank = "";

char *nil = "NIL";
char *t = "T";

// prototypy funkcí
void komp(char *res);
void vyraz(char *out);
void quote_komp(char *res);

void quote_list(char *res)
{
    atom act;
    char *var = (char *) malloc(128);
    char *start = var;

    *var++ = '(';

    // první symbol je bez uvozující mezery
    lexa_get(&act);
    quote_komp(res);
    strcpy(var, res);
    var += strlen(res);

    while (lexa_next(&act) && (act.type == AT_VALUE || act.type == AT_IDENT ||
                               act.type == AT_LBRACKET || act.type == AT_OPERATOR))
    {
        quote_komp(res);
        *var++ = ' ';

        strcpy(var, res);
        var += strlen(res);
    }

    strcpy(var,")\0");

    strcpy(res, start);
    free(start);
}

// vrací jeden symbol, ale nevyhodnocuje
void quote_komp(char *res)
{
    atom act;

    lexa_get(&act);

    switch (act.type)
    {
    case AT_VALUE:
        sprintf(res, "%d", *((int *) act.data));
        break;
    case AT_IDENT:
    case AT_OPERATOR:
        strcpy(res, act.data);
        break;
    case AT_LBRACKET:
        lexa_next(&act);
        quote_list(res);
        break;
    }
}

// vstup do výpisu
void quote(char *res)
{
    atom act;

    lexa_get(&act);

    if (*act.data == '\'' || equals(act.data, qe, 0))
    {
        lexa_next(NULL);
        quote_komp(res);
    }
    else
    {
        komp(res);
    }
}

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

int small(int a, int b)
{
    return a < b;
}

int big(int a, int b)
{
    return a > b;
}

int smalleq(int a, int b)
{
    return a <= b;
}

int bigeq(int a, int b)
{
    return a >= b;
}

int zero(int a, int b)
{
    return 0;
}

// Vrátí ukazatel na paritní závorku, nebo na
// poslední znak.
char *parite(char *ptr)
{
    int op = 1;
    ptr++;

    while (op > 0 && *ptr != 0)
    {
        if (*ptr == '(')
            op++;
        else if (*ptr == ')')
            op--;
        ptr++;
    }
    return ptr - 1;
}

// Jedná se o výraz složený z více symbolů?
bool isbrclist(char *start, char *end)
{
    bool res = false;

    if (start == end)
        return false;

    while (start < end)
    {
        if (isspace(*start))
            return true;
        start++;
    }

    return res;
}

void komp(char *res)
{
    atom act;
    int *var_v = NULL;
    char *var_n = NULL;
    int i;
    bool found;

    lexa_get(&act);

    if (act.type == AT_VALUE)
    {
        sprintf(res, "%d", *((int *) act.data));
    }
    else if (act.type == AT_LBRACKET)
    {
        lexa_next(&act);
        switch (act.type)
        {
        case (AT_OPERATOR):
            vyraz(res);
            break;

        case (AT_VALUE):
        case (AT_IDENT):
        case (AT_LBRACKET):
            komp(res);
            break;
        case (AT_RBRACKET):
            strcpy(res, nil);
            break;
        }
    }
    else if (act.type == AT_RBRACKET)
    {
        strcpy(res, qt);
    }
    // identifikátor
    else if (act.type == AT_IDENT || act.type == AT_UNKNOWN)
    {
        if (equals(act.data, qt, 0))
            strcpy(res, qt);
        else if (equals(act.data, set, 0))
        {
            lexa_next(NULL);
            komp(res);
            var_n = (char *) malloc(strlen(res) + 1);
            strcpy(var_n, res);

            lexa_next(NULL);
            komp(res);
            var_v = (int *) malloc(sizeof(int));
            *var_v = strtol(res, NULL, 10);

            push(var_v, var_n);

            //	  sprintf(res, "%d", *var_v);
        }
        // car - vrátí první prvek seznamu
        else if (equals(act.data, car, 0))
        {
            lexa_next(&act);
            komp(res);
            if (equals(res, nil, 0))
            {
                printf("Prázdný seznam.");
                *res = '\0';
                return;
            }

            char *newres = (char *) malloc(68);
            char *start = res + 1;
            char *end = res + 1;

            if (*start == '(')
            {
                end = parite(start);
                end++;
            }
            else
            {
                while(!isspace(*end))
                    end++;
            }

            lexa_next(NULL);
            // vytvoříme nový řetězec
            memcpy(newres, start, end - start);
            *(newres + (end - start)) = 0;

            // ten zapíšeme do výsledku
            strcpy(res, newres);
            free(newres);
        }
        // Vrátí všechny prvky seznamu kromě prvního.
        else if (equals(act.data, cdr, 0))
        {
            lexa_next(&act);
            komp(res);
            if (equals(res, nil, 0))
            {
                printf("Prázdný seznam.");
                *res = '\0';
            }

            char *newres = (char *) malloc(68);

            // začínáme následujícím symbolem za
            // závorkou.
            char *start = res + 1;
            char *end = res + 1;

            if (*start == '(')
            {
                start = parite(start);
                start++;
            }
            else
            {
                // přesunu se na konec prvního symbolu
                while(!isspace(*start))
                    start++;
            }

            // přeskočím bílé znaky
            while(isspace(*start))
                start++;

            // najít konec seznamu
            end = parite(start);

            lexa_next(NULL);
            // vytvoříme nový řetězec

            if (isbrclist(start, end))
            {
                *newres = '(';
                memcpy(newres + 1, start, end - start);
                *(newres + (end - start) + 1) = ')';

                // +2 protože jsme přidali dva znaky navíc
                *(newres + (end - start) + 2) = 0;
            }
            else
            {
                memcpy(newres, start, end - start);
                *(newres + (end - start)) = 0;
            }

            // ten zapíšeme do výsledku
            strcpy(res, newres);
            free(newres);
        }
        // quote - Vypíše a nevyhodnotí
        else if (equals(act.data, qe, 0) || *act.data == '\'')
        {
            quote(res);
        }
        // Nápověda
        else if (equals(act.data, hp, 0) || equals(act.data, about, 0))
        {
            help();
            strcpy(res, blank);
        }
        else if (equals(act.data, list, 0)) // seznam
        {
            lexa_next(&act);
            komp(res);

            if (equals(res, nil, 0) || equals(res, qt, 0))
            {
                printf("Prázdný seznam");
                strcpy(res, blank);
                return;
            }

            var_n = (char *) malloc(128);
            char *start = var_n;

            *var_n++ = '(';

            strcpy(var_n, res);
            var_n += strlen(res);

            while (lexa_next(&act) && (act.type == AT_VALUE || act.type == AT_IDENT ||
                                       act.type == AT_LBRACKET))
            {
                komp(res);
                *var_n++ = ' ';

                strcpy(var_n, res);
                var_n += strlen(res);
            }

            strcpy(var_n,")\0");

            strcpy(res, start);
            free(start);
        }
        else if (equals(act.data, nil, 0))
            strcpy(res, nil);
        else // Musí to být tedy proměnná.
        {
            begin();
            found = false;
            for (i = 0; i < count(); i++)
            {
                // seznam, který byl naplněn příkazem set
                next(&var_v, &var_n);
                if (equals(var_n, act.data, 0))
                {
                    sprintf(res, "%d", *var_v);
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                printf("(EE) Proměnná \"%s\" nenalezena\n", act.data);
                strcpy(res, blank);
            }
        }
    }
    /*
      else if (act.type = AT_UNKNOWN)
      {

      }
    */
}

void vyraz(char *out)
{
    atom act;
    int res = 0;
    bool bres = true;
    int (*op)(int, int) = zero;
    int type = UNKN;
    char buf[20];

    lexa_get(&act);

    switch (*act.data)
    {
    case '+':
        op = addii;
        res = 0;
        type = ARIT;
        break;
    case '-':
        op = subii;
        type = ARIT;
        lexa_next(&act);
        if (act.type == AT_VALUE || act.type == AT_IDENT ||
                act.type == AT_LBRACKET)
        {
            komp(buf);
            res = strtol(buf, NULL, 10);
        }
        else
            die("(EE) Nelze vyhodnotit");
        break;
    case '*':
        op = mulii;
        type = ARIT;
        res = 1;
        break;
    case '/':
        // to značí normální dělení
        if (act.data[1] == 0)
        {
            op = divii;
            type = ARIT;
            lexa_next(&act);
            if (act.type == AT_VALUE || act.type == AT_IDENT
                    || act.type == AT_LBRACKET)
            {
                komp(buf);
                res = strtol(buf, NULL, 10);
            }
            else
                die("(EE) Nelze dělit");
        }
        else if (act.data[1] == '=')     // operátor nerovnosti
        {
            op = ncomp;
            type = BOOL;
        }
        break;
    case '=':
        op = comp;
        type = BOOL;
    case '<':
        // menší než
        if (act.data[1] == 0)
        {
            op = small;
            type = BOOL;
            lexa_next(&act);
            if (act.type == AT_VALUE || act.type == AT_IDENT ||
                    act.type == AT_LBRACKET)
            {
                komp(buf);
                res = strtol(buf, NULL, 10);
            }
            else
                die("(EE) Nelze porovnat");
        }
        else if (act.data[1] == '=')     // menší nebo rovno
        {
            op = smalleq;
            type = BOOL;
        }

        break;
    case '>':
        if (act.data[1] == 0)
        {
            op = big;
            type = BOOL;
            lexa_next(&act);
            if (act.type == AT_VALUE || act.type == AT_IDENT ||
                    act.type == AT_LBRACKET)
            {
                komp(buf);
                res = strtol(buf, NULL, 10);
            }
            else
                die("(EE) Nelze porovnat");
        }
        else if (act.data[1] == '=')     // větší nebo rovno
        {
            op = bigeq;
            type = BOOL;
        }
    }

    // inicializace počáteční hodnoty před vyhodnocováním
    if (type == BOOL)
    {
        lexa_next(&act);
        if (act.type == AT_VALUE || act.type == AT_IDENT ||
                act.type == AT_LBRACKET)
        {
            komp(buf);
            res = strtol(buf, NULL, 10);
        }
        else
            printf("(EE) Neplatný výraz: %s\n", act.data);
    }

    lexa_next(&act);
    while (act.type == AT_VALUE || act.type == AT_IDENT
            || act.type == AT_LBRACKET)
    {
        if (type == ARIT)
        {
            komp(buf);
            res = op(res, strtol(buf, NULL, 10));
        }
        else if (type == BOOL)
        {
            komp(buf);
            bres &= op(res, strtol(buf, NULL, 10));
        }
        if (!lexa_next(&act))
            break;
    }

    if (type == ARIT)
    {
        sprintf(out, "%d", res);
    }
    else
    {
        strcpy(out, bres ? t : nil);
    }
}

int start()
{
    atom act;
    char res[128];

    // do some initialization
    res[0] = 'Q';
    res[1] = 'U';
    res[2] = 'I';
    res[3] = 'T';
    res[4] = 0;

    lexa_next(&act);

    //  printf("první znak: %d", act.type);

    switch (act.type)
    {
    case AT_OPERATOR:
        vyraz(res);
        break;
    default:
        komp(res);
        if (equals(res, qt, 0))
        {
            return 0;
        }
        break;
    }

    printf("%s\n",res);
    return 1;
}
