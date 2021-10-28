#include <stdint.h>

extern "C" void isyscall(uint8_t syscall,uint64_t arg1,uint64_t arg2,uint64_t arg3);

extern "C" void moldMain() {
    const char* string = "Welcome to moldOS!\nBy Moldu' (Nov. 2020 - Oct. 2021)";
    for(int i = 0;string[i] != 0;i++)
        isyscall(0x01,string[i],1,0);
    isyscall(0xFF,2021,0,0);
    while(1);
}