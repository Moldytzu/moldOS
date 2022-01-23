#include <mlib.h>

size_t strlen(const char *str)
{
    int i = 0;
    while(str[i++]);
    return i;
}