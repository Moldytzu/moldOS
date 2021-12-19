#pragma once
#include <stdint.h>
#include <stddef.h>

#include <memory/PageTableManager.h>
#include <memory/PageFrameAllocator.h>

struct HeapSegHdr
{
    size_t length;
    HeapSegHdr* next;
    HeapSegHdr* last;
    bool free;
    void volatile CombineForward();
    void volatile CombineBackward();
    HeapSegHdr* volatile Split(size_t splitLength);
} __attribute__((packed));

void volatile InitializeHeap(void* heapAddress, size_t pageCount);

void* volatile malloc(size_t size);
void volatile free(void* address);

void volatile ExpandHeap(size_t length);

inline void* operator new(size_t size)
{
    return malloc(size);
}
inline void* operator new[](size_t size)
{
    return malloc(size);
}

inline void operator delete(void* p)
{
    free(p);
}