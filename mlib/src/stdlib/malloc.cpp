#include <mlib.h>

void* malloc(size_t size)
{
    return (void*)sys_call(SYS_ALLOCATEMEMORY,size/4096+1,0,0,0);
}