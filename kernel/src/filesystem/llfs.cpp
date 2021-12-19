#include <filesystem/llfs.h>

LLFSEntry* LLFSOpenFile(LLFSHeader* fs,const char* filename)
{
    LLFSEntry* firstEntry = (LLFSEntry*)((uint64_t)fs+sizeof(LLFSHeader));
    for(int i = 0; i<fs->Entries; i++)
    {
        if(memcmp(firstEntry->Filename,filename,strlen(filename)) == 0)
        {
            return firstEntry;
        }
        firstEntry = (LLFSEntry*)((uint64_t)firstEntry+sizeof(LLFSEntry)+firstEntry->FileSize);
    }
    return 0;
}

LLFSEntry* LLFSOpenFile(const char* filename)
{
    return LLFSOpenFile(LLFSSource,filename);
}

void* LLFSVFSHelperReader(const char* filename)
{
    return LLFSReadFile(LLFSOpenFile(filename+1));
}

void* LLFSReadFile(LLFSEntry* entry)
{
    return (void*)((uint64_t)entry+sizeof(LLFSEntry));
}

uint64_t LLFSGetFileSystemSize(LLFSHeader* fs)
{
    LLFSEntry* firstEntry = (LLFSEntry*)((uint64_t)fs+sizeof(LLFSHeader));
    uint64_t fsize = sizeof(LLFSHeader);
    for(int i = 0; i<fs->Entries; i++)
    {
        fsize += sizeof(LLFSEntry)+firstEntry->FileSize;
        firstEntry = (LLFSEntry*)((uint64_t)firstEntry+sizeof(LLFSEntry)+firstEntry->FileSize);
    }
    return fsize;
}

int LLFSCheck(LLFSHeader* fs)
{
    if(fs->Signature[0] != 'L' || fs->Signature[1] != 'L' || fs->Signature[2] != 'F' || fs->Signature[3] != 'S') return 0;
    if(fs->Version == 2) return 1;
    return 0;
}

void LLFSMap(LLFSHeader* fs)
{
    LLFSEntry* firstEntry = (LLFSEntry*)((uint64_t)fs+sizeof(LLFSHeader));
    uint64_t fsize = sizeof(LLFSHeader);
    for(int i = 0; i<fs->Entries; i++)
    {
        GlobalTableManager.MapUserspaceMemory(firstEntry);
        firstEntry = (LLFSEntry*)((uint64_t)firstEntry+sizeof(LLFSEntry)+firstEntry->FileSize);
    }
}