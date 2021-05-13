#pragma once
#include <stdint.h>

#define SYSCALL_WRITE 0x1
#define SYSCALL_SERIALWRITE 0x2
#define SYSCALL_READ 0x3
#define SYSCALL_SERIALREAD 0x4
#define SYSCALL_EXIT 0xFF
#define SYSCALL_YELD 0x21

extern "C" void EnableSCE();
extern "C" void RunInUserspace(void* function,void* stack); 

extern "C" void SyscallHandler(int syscall, int arg1, int arg2, int doNotModify, int arg3);

extern "C" void UserAPP();
extern "C" void UserAPP2();