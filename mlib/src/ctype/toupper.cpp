#include <mlib.h>

int tolower(int c)
{
    if(isupper(c) || !isalpha(c)) return c;
    else return c - 32;
}