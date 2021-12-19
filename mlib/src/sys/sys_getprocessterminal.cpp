#include <mlib.h>

uint64_t sys_getprocessterminal()
{
    return sys_call(SYS_GETPROCESSTERMINAL,0,0,0,0);
}