#include <mlib.h>

void exit(uint64_t code) {
    isyscall(0xFF,code,0,0);
    while(1);
}