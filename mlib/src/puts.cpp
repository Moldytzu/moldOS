#include <mlib.h>

void puts(const char* str) {
    for(int i = 0;str[i] != 0;i++)
        isyscall(SYS_WRITEC,str[i],COUT,0);
}