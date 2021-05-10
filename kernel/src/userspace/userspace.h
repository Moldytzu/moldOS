#pragma once
#include <stdint.h>

#define SYSCALL_WRITE 0x1
#define SYSCALL_SERIALWRITE 0x2
#define SYSCALL_EXIT 0xFF

extern "C" void EnableSCE();
extern "C" void RunInUserspace(void* function,void* stack); 
extern "C" void UserAPP();
extern "C" void SyscallHandler(int syscall, int arg1, int arg2, int doNotModify, int arg3);