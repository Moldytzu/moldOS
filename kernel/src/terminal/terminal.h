#pragma once
#include <stdint.h>
#include <stddef.h>
#include <memory/PageFrameAllocator.h>

struct VirtualTerminal {
    void init(uint32_t len);
    void append(const char* string);
    const char* buffer;
    uint32_t maxLen;
    uint32_t index;
    bool initialized;
};

extern VirtualTerminal VirtualTerminals[];