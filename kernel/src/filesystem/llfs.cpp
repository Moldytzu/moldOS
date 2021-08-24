#include "llfs.h"
#include "../memory/memory.h"

LLFSEntryV1* LLFSOpenFile(LLFSHeader* fs,const char* filename) {
    LLFSEntryV1* firstEntry = (LLFSEntryV1*)((uint64_t)fs+sizeof(LLFSHeader));
    for(int i = 0;i<fs->Entries;i++) {
        if(memcmp(firstEntry->Filename,filename,16) == 0) {
            return firstEntry;
        }
        firstEntry = (LLFSEntryV1*)((uint64_t)firstEntry+sizeof(LLFSEntryV1)+firstEntry->FileSize);
    }
    return 0;
}

void* LLFSReadFile(LLFSEntryV1* entry) {
    return (void*)((uint64_t)entry+sizeof(LLFSEntryV1));
}

uint64_t LLFSGetFileSystemSize(LLFSHeader* fs) {
    LLFSEntryV1* firstEntry = (LLFSEntryV1*)((uint64_t)fs+sizeof(LLFSHeader));
    uint64_t fsize = sizeof(LLFSHeader);
    for(int i = 0;i<fs->Entries;i++) {
        fsize += sizeof(LLFSEntryV1)+firstEntry->FileSize;
        firstEntry = (LLFSEntryV1*)((uint64_t)firstEntry+sizeof(LLFSEntryV1)+firstEntry->FileSize);
    }
    return fsize;
}

int LLFSCheck(LLFSHeader* fs) {
    if(fs->Signature[0] != 'L' || fs->Signature[1] != 'L' || fs->Signature[2] != 'F' || fs->Signature[3] != 'S') return 0;
    if(fs->Version == 1 || fs->Version == 2) return 1;
    return 0;
}

void LLFSMap(LLFSHeader* fs) {
    LLFSEntryV1* firstEntry = (LLFSEntryV1*)((uint64_t)fs+sizeof(LLFSHeader));
    uint64_t fsize = sizeof(LLFSHeader);
    for(int i = 0;i<fs->Entries;i++) {
        GlobalTableManager.MapUserspaceMemory(firstEntry);
        firstEntry = (LLFSEntryV1*)((uint64_t)firstEntry+sizeof(LLFSEntryV1)+firstEntry->FileSize);
    }
}