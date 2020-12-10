#pragma once
#include "efiMemory.h"
#include "../misc/bitmap.h"
#include <stdint.h>

class PageFrameAllocator
{
public:
    void readMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t nMapSize, size_t nMapDescSize);
    Bitmap PageBitmap;
private:
    void InitBitmap(size_t bmapsize, void* bufferAddress);
};