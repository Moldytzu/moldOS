#pragma once
#include <stdint.h>

#define SYSCALL_WRITE 0x1
#define SYSCALL_READ 0x2
#define SYSCALL_UPDATESCREEN 0x3
#define SYSCALL_EXIT 0xFF

#define USERSPACE_STACK_SIZE 4096

#define COUT 1 //console out
#define SOUT 2 //serial out

#define ERROR_UNKNOWN_SYSCALL 2550
#define ERROR_ACCESS_DENIED 5435

extern "C" void EnableSCE();
extern "C" void RunInUserspace(void* function,void* stack); 

extern "C" void SyscallHandler(int syscall, int arg1, int arg2, int doNotModify, int arg3);

