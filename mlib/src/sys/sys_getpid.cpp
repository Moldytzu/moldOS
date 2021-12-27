#include <mlib.h>

uint64_t sys_getpid()
{
    return sys_call(SYS_GETPID,0,0,0,0);
}