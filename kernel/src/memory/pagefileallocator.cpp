#include "pagefileallocator.h"

uint64_t freeMemory;
uint64_t reservedMemory;
uint64_t usedMemory;
bool Initialized = false;

void PageFrameAllocator::readMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t nMapSize, size_t nMapDescSize) {
    if(Initialized) return;
    Initialized = true;

    uint64_t nMapEntries = nMapSize / nMapDescSize;

    void* largestSeg = NULL;
    size_t largestSegSize = 0;

    for(int i = 0;i< nMapEntries;i++) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i*nMapDescSize));
        if(desc->type == 7) {
            if(desc->numPages * 4096 >largestSegSize) {
                largestSeg = desc->physAddr;
                largestSegSize = desc->numPages * 4096;
            }
        }
    }

    uint64_t memorySize = GetMemorySize(mMap, nMapEntries, nMapDescSize);
    freeMemory = memorySize;

    uint64_t bitmsize = memorySize / 4096 / 8 + 1;

    InitBitmap(bitmsize, largestSeg);
}


void PageFrameAllocator::InitBitmap(size_t bmapsize, void* bufferAddress) {
    PageBitmap.Size = bmapsize;
    PageBitmap.Buffer = (uint8_t*)bufferAddress;
    for(int i =0 ;i< bmapsize;i++) {
        *(uint8_t*)(PageBitmap.Buffer + i) = 0;
    }
}