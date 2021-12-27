#include <mlib.h>

uint64_t sys_getpidbyname(const char* name)
{
    return sys_call(SYS_GETPIDBYNAME,(uint64_t)(void*)name,0,0,0);
}