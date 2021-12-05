#pragma once
#include <libc/va_args.h>
#include <drivers/display/display.h>
#include <libc/stdio/cstring.h>
#include <terminal/terminal.h>

void printf(const char* str, ...);
void printf(const char* str, va_list list);
void printf(uint32_t vt,const char* str, ...);