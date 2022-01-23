#include <mlib.h>

void puts(const char* str)
{
    sys_call(SYS_WRITEP,(uint64_t)(void*)str,COUT,strlen(str),0);
}