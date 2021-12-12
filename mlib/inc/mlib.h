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
#define SYS_GETTERMINALOUTPUT 0x4
#define SYS_GETPROCESSTERMINAL 0x5
#define SYS_EXIT 0xFF

#ifdef __cplusplus
extern "C" uint64_t sys_call(uint8_t syscall,uint64_t arg1,uint64_t arg2,uint64_t arg3);
#else
extern uint64_t sys_call(uint8_t syscall,uint64_t arg1,uint64_t arg2,uint64_t arg3);
#endif
void exit(uint64_t code);
const char* sys_getterminaloutput(uint64_t terminal);
uint64_t sys_getprocessterminal();