#include <stdint.h>

extern "C" void isyscall(uint8_t syscall,uint64_t arg1,uint64_t arg2,uint64_t arg3);
void puts(const char* str);
void exit(uint64_t code);