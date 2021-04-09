#pragma once
#include <stdint.h>
#include "../memory/PageFrameAllocator.h"
#include "../libc/stdio.h"
#include "../drivers/display/displaydriver.h"

struct GDTDescriptor {
    uint16_t Size;
    uint64_t Offset;
} __attribute__((packed));

struct GDTEntry {
    uint16_t Limit0;
    uint16_t Base0;
    uint8_t Base1;
    uint8_t AccessByte;
    uint8_t Limit1_Flags;
    uint8_t Base2;
}__attribute__((packed));

struct GDT {
    GDTEntry Null; //0x00
    GDTEntry KernelCode; //0x08
    GDTEntry KernelData; //0x10
    GDTEntry UserNull;
    GDTEntry UserCode;
    GDTEntry UserData;
    GDTEntry OVMFCode;
    GDTEntry OVMFData;
    GDTEntry TSSLow;
    GDTEntry TSSHigh;
} __attribute__((packed)) 
__attribute((aligned(0x1000)));

struct TSS {
	uint32_t reserved0;
	uint64_t rsp0;	// ring stacks
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t reserved1;
	uint64_t ist1;	// interrupt stack tables
	uint64_t ist2;
	uint64_t ist3;
	uint64_t ist4;
	uint64_t ist5;
	uint64_t ist6;
	uint64_t ist7;
	uint64_t reserved2;
	uint16_t reserved3;
	uint16_t io_map_base;
} __attribute__((packed));

struct TSSDescriptor {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t	base_mid;
	uint8_t	access;
	uint8_t	flags;
	uint8_t	base_mid2;
	uint32_t base_high;
	uint32_t reserved;
} __attribute__((packed));

extern GDT DefaultGDT;

void TSSInit(void* stack);
extern "C" void GDTSetKernelStack(uint64_t reserved,void* stack);

extern "C" void LoadGDT(GDTDescriptor* gdtDescriptor);