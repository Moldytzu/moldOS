#include <stdint.h>
#include <stddef.h>

#include <stdio.h>
#include <ctype.h>

#include "assert.h"

#define isyscall sys_call

#define SYS_EXIT 0xFF
#define SYS_WRITEC 0x1

extern "C" uint64_t sys_call(uint8_t syscall,uint64_t arg1,uint64_t arg2,uint64_t arg3);
void exit(uint64_t code);