#include <mlib.h>

int tolower(int c) {
    if(islower(c) || !isalpha(c)) return c;
    else return c + 32;
}