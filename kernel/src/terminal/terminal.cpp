#include <terminal/terminal.h>

VirtualTerminal VirtualTerminals[0x800]; //2048 terminals

void VirtualTerminal::init(uint32_t len) {
    maxLen = len;
    index = 0;
    buffer = (const char*)GlobalAllocator.RequestPages(len/4096);
}

void VirtualTerminal::append(const char* string) {
    asmmemcpy((void*)((uint64_t)buffer+index),string,strlen(string)+1);
    index += strlen(string)+1;
}