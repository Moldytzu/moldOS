#pragma once
#include "../filesystem/llfs.h"
#include <elf.h>

void* LoadELFExecutable(LLFSHeader* fs,const char* file);