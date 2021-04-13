#include "userspace.h"
#include "../libc/stdio.h"
#include "../drivers/display/displaydriver.h"

void DoWrite(char* str) {
    printf("%u",(uint64_t)str);
    GlobalDisplay->update();
}

void SYSHandle(uint64_t a,uint64_t b,uint64_t c) {
    switch (a)
    {
    case 0x1:
        DoWrite((char*)b);
    default:
        break;
    }
    //printf();
}