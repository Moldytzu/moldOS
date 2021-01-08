#pragma once
#include <stdint.h>

#define IDT_TA_IntreruptGate 0x8E
#define IDT_TA_CallGate 0x8C
#define IDT_TA_TrapGate 0x8F

struct IDTDescriptorEntry {
    uint16_t Offset0;
    uint16_t Selector;
    uint8_t IST;
    uint8_t Type_Attributes;
    uint16_t Offset1;
    uint32_t Offset2;
    uint32_t Ignore;
    void setOffset(uint64_t Offset);
    uint64_t getOffset();
};

struct IDTR {
    uint16_t Limit;
    uint64_t Offset;
} __attribute__((packed));