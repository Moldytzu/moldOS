#pragma once
#include <stdint.h>
#include "../memory/PageTableManager.h"

struct LLFSHeader {
    char Signature[4];
    uint8_t Version;
    uint64_t Entries;
    char Padding[4];
} __attribute__((packed)); 

struct LLFSEntryV1 {
    char Filename[16];
    uint64_t FileSize;
    char Padding;
} __attribute__((packed));

struct LLFSEntryHeader {
    uint8_t Type;
} __attribute__((packed));

struct LLFSEntryFile {
    uint8_t FilenameLen;
    char* Filename;
    uint8_t DirectoryLen;
    char* Directory;
    uint8_t attributes;
    uint64_t FileSize;
} __attribute__((packed));

struct LLFSEntryMeta {
    uint8_t Year;
    uint8_t Month;
    uint8_t Day;
} __attribute__((packed));

LLFSEntryV1* LLFSOpenFile(LLFSHeader* fs,const char* filename);
void* LLFSReadFile(LLFSEntryV1* entry);
uint64_t LLFSGetFileSystemSize(LLFSHeader* fs);
int LLFSCheck(LLFSHeader* fs);
void LLFSMap(LLFSHeader* fs);