#include <stdint.h>
#include <stddef.h>

#include <stdio.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <string.h>

#include "assert.h"

#define isyscall sys_call

#define SYS_WRITEC 0x1
#define SYS_WRITEP 0xA
#define SYS_READ 0x2
#define SYS_OPEN 0x3
#define SYS_GETTERMINALOUTPUT 0x4
#define SYS_GETPROCESSTERMINAL 0x5
#define SYS_ALLOCATEMEMORY 0x6
#define SYS_GETPIDBYNAME 0x7
#define SYS_GETPID 0x8
#define SYS_GETTERMINALBYPID 0x9
#define SYS_EXIT 0xFF

#ifdef __cplusplus
extern "C" uint64_t sys_call(uint8_t syscall,uint64_t arg1,uint64_t arg2,uint64_t arg3,uint64_t arg4);
#else
extern uint64_t sys_call(uint8_t syscall,uint64_t arg1,uint64_t arg2,uint64_t arg3,uint64_t arg4);
#endif
void exit(uint64_t code);
const char* sys_getterminaloutput(uint64_t terminal);
uint64_t sys_getprocessterminal();
uint64_t sys_getpid();
uint64_t sys_getpidbyname(const char* name);
uint64_t sys_getterminalbypid(uint64_t pid);