#include <mlib.h>

void puts(const char* str) {
    for(int i = 0;str[i] != 0;i++)
        isyscall(0x1,str[i],1,0);
}