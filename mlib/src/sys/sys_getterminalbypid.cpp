#include <mlib.h>

uint64_t sys_getterminalbypid(uint64_t pid)
{
    return sys_call(SYS_GETTERMINALBYPID,pid,0,0,0);
}