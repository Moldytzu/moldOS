#include <mlib.h>

void putchar(int ch)
{
    sys_call(SYS_WRITEC,ch,COUT,0,0);
}