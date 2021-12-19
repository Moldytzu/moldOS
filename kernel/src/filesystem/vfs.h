#pragma once
#include <stddef.h>
#include <stdint.h>
#include <memory/PageFrameAllocator.h>
#include <filesystem/llfs.h>

#define VFS_SOURCE_RAMFS 1

struct FileDescriptor
{
    const char path[368];
    uint8_t source = 0;
};

inline uint8_t VFSSource = 0;

FileDescriptor* VFSOpenFile(const char* path);
void* VFSReadFile(FileDescriptor* file);
uint64_t VFSSizeFile(FileDescriptor* file);