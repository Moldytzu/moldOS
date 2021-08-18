#pragma once
#include <stdint.h>
#include "gdt.h"
#include "../memory/memory.h"

/*
This file's author is KeepKonect.
All credits go to him
*/

#define IST_POINTER         0x700000
#define IST_SIZE            0x100000


struct TSS{
    uint32_t Reserved0;

    //RSP
    uint64_t RSP[3];

    uint64_t Reserved1;

    //IST
    uint64_t IST[7];

    uint64_t Reserved2;
    uint16_t Reserved3;

    uint16_t IOPBOffset;
}__attribute__((packed));

void TSSInit();
uint16_t TSSInstall(int numCPU);

extern "C" void TSSSetStack(int numCPU, void* stack);
extern "C" uint64_t TSSGetStack(int numCPU);

extern "C" void LoadTSS(uint16_t tss);