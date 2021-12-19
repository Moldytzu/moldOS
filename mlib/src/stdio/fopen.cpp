#include <mlib.h>

FILE *fopen(const char* file, const char* mode)
{
    return (FILE*)(void*)sys_call(SYS_OPEN,(uint64_t)(void*)file,(uint64_t)(void*)mode,0,0);
}