#pragma once
#include <stdint.h>
#include <memory/PageTableManager.h>
#include <memory/memory.h>

struct LLFSHeader
{
    char Signature[4];
    uint8_t Version;
    uint64_t Entries;
    char Padding[4];
} __attribute__((packed));

struct LLFSEntry
{
    char Filename[16];
    uint64_t FileSize;
    char Padding;
} __attribute__((packed));

inline LLFSHeader* LLFSSource;

LLFSEntry* LLFSOpenFile(LLFSHeader* fs,const char* filename);
LLFSEntry* LLFSOpenFile(const char* filename);
void* LLFSVFSHelperReader(const char* filename);
void* LLFSReadFile(LLFSEntry* entry);
uint64_t LLFSGetFileSystemSize(LLFSHeader* fs);
int LLFSCheck(LLFSHeader* fs);
void LLFSMap(LLFSHeader* fs);