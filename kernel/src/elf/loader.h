#pragma once
#include "../filesystem/llfs.h"
#include <elf.h>

void* LoadELFExecutable(LLFSHeader* fs,const char* file, bool pie);
int CheckELF(LLFSHeader* fs,const char* file);