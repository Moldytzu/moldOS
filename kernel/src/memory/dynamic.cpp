#include "dynamic.h"

void* malloc(size_t size) {
    if(size < 4096)
        return GlobalAllocator.RequestPage();
    else {
        void* base = GlobalAllocator.RequestPage();
        GlobalAllocator.LockPages(base,(uint64_t)((size/4096)+1));
        return base;
    }
}

void* calloc(size_t num,size_t size) {
    void* base = GlobalAllocator.RequestPage();
    if(num > 4096) GlobalAllocator.LockPages(base,(uint64_t)((num/4096)+1));
    for(uint64_t i = (uint64_t)base;i<num;i+=size) {
        *(uint64_t*)i = 0;
    }
    return base;
}

void free(void* adress) {
    GlobalAllocator.FreePage(adress);
    //todo: make to work with buffers larger than 4096
}