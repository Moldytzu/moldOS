#pragma once
#include <filesystem/llfs.h>
#include <elf/elf.h>
#include <stddef.h>
#include <libc/stdio.h>
#include <memory/PageTableManager.h>
#include <memory/heap.h>
#include <filesystem/vfs.h>

void* LoadELFExecutable(LLFSHeader* fs,const char* file, bool pie);
int CheckELF(LLFSHeader* fs,const char* file);