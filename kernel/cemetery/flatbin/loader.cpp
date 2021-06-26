#include "loader.h"
#include "../memory/PageFrameAllocator.h"

void* LoadFlatBinary(LLFSHeader* fs,const char* file) {
    void* fileContents = LLFSReadFile(LLFSOpenFile(fs,"llinit.llexec   "));
    void* fileAllocated = GlobalAllocator.RequestPage();
    memcpy(fileAllocated,fileContents,4096);
    GlobalTableManager.MapUserspaceMemory(fileAllocated);
    return fileAllocated;
}