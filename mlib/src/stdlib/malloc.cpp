#include <mlib.h>

void* malloc(size_t size)
{
    uint64_t pages = size/4096;
    if(size % 4096 != 0) pages++;
    return (void*)sys_call(SYS_ALLOCATEMEMORY,pages,0,0,0);
}