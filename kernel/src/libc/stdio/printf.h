#pragma once
#include <libc/va_args.h>
#include <drivers/display/display.h>
#include <libc/stdio/cstring.h>
#include <terminal/terminal.h>

void printf(const char* str, ...);