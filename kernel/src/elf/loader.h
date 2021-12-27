#pragma once
#include <filesystem/llfs.h>
#include <elf/elf.h>
#include <stddef.h>
#include <libc/stdio.h>
#include <memory/ptm.h>
#include <memory/heap.h>
#include <filesystem/vfs.h>

void* LoadELFExecutable(const char* file, bool pie);
int CheckELF(Elf64_Ehdr* header);