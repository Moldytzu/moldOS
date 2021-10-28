#include <mlib.h>

void exit(uint64_t code) {
    isyscall(SYS_EXIT,code,0,0);
    while(1);
}