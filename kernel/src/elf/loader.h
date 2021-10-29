#pragma once
#include <filesystem/llfs.h>
#include <elf.h>
#include <stddef.h>
#include <libc/stdio.h>
#include <memory/PageTableManager.h>
#include <memory/heap.h>

void* LoadELFExecutable(LLFSHeader* fs,const char* file, bool pie);
int CheckELF(LLFSHeader* fs,const char* file);