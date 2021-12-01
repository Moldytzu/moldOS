#include <mlib.h>

int isgraph(int c) {
    return (c >= '!') && (c <= '~');
}