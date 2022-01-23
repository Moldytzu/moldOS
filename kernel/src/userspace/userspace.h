#pragma once
#include <stdint.h>
#include <libc/stdio.h>
#include <drivers/display/display.h>
#include <io/serial.h>
#include <drivers/keyboard/keyboard.h>
#include <misc/logging/log.h>
#include <scheduling/taskmgr.h>
#include <filesystem/vfs.h>

#define SYSCALL_WRITEC 0x1
#define SYSCALL_WRITEP 0xA
#define SYSCALL_READ 0x2
#define SYSCALL_OPEN 0x3
#define SYSCALL_GETTERMINALOUTPUT 0x4
#define SYSCALL_GETPROCESSTERMINAL 0x5
#define SYSCALL_ALLOCATEMEMORY 0x6
#define SYSCALL_GETPIDBYNAME 0x7
#define SYSCALL_GETPID 0x8
#define SYSCALL_GETTERMINALBYPID 0x9
#define SYSCALL_EXIT 0xFF

#define USERSPACE_STACK_SIZE 4096

#define COUT 1 //console out
#define SOUT 2 //serial out

#define ERROR_UNKNOWN_SYSCALL 2550
#define ERROR_ACCESS_DENIED 5435

extern "C" void EnableSCE();
extern "C" void RunInUserspace(void* function,void* stack);

extern "C" uint64_t SyscallHandler(uint64_t syscall, uint64_t arg1, uint64_t arg2, uint64_t doNotModify, uint64_t arg3, uint64_t arg4);

