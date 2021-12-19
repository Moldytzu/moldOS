#include <mlib.h>

void fread(void* buffer, size_t sizeData, size_t numbersData,FILE *file)
{
    sys_call(SYS_READ,(uint64_t)buffer,(uint64_t)(void*)file,sizeData*numbersData,0);
}