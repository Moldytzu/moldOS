#include <mlib.h>

int isspace(int c)
{
    return (c == ' ') || (c == '\n') || (c == '\r') || (c == '\t');
}