#pragma once
#include <stddef.h>
#include <stdint.h>
#include <memory/PageFrameAllocator.h>
#include <filesystem/llfs.h>
#include <memory/heap.h>

#define VFS_SOURCE_RAMFS 1

#define VFS_MAX_DESCRIPTORS 0x19F

struct FileDescriptor
{
    char path[368] = {0};
    uint8_t source = 0;
    FileDescriptor();
};

inline uint8_t VFSSource = 0;
inline uint64_t VFSLastDescriptor = 0;
inline FileDescriptor** VFSDescriptors;

FileDescriptor* VFSOpenFile(const char* path);
void* VFSReadFile(FileDescriptor* file);
uint64_t VFSSizeFile(FileDescriptor* file);
void VFSInit();
void VFSAdd(FileDescriptor toAdd);
void VFSRemove(const char* path);