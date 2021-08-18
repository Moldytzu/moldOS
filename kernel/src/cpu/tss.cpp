#include "tss.h"

/*
This file's author is KeepKonect.
All credits go to him
*/

static TSS TSSdescriptors[1];

void TSSInit(){
    memset(TSSdescriptors, 0, sizeof(TSS) * 1);
}

uint16_t TSSInstall(int numCPU){
    uint64_t tss_base = (uint64_t)&TSSdescriptors[numCPU];
    memset((void *)tss_base, 0, sizeof(TSS));

    uint16_t location = gdtInstallTSS(tss_base, sizeof(TSS));
    TSSdescriptors[numCPU].IOPBOffset = sizeof(TSS);
    return location;
}

void TSSSetStack(int numCPU, void* stack){
    TSSdescriptors[numCPU].RSP[0] = (uint64_t)stack;
}

uint64_t TSSGetStack(int numCPU){
    return TSSdescriptors[numCPU].RSP[0];
}

TSS* TSSGet(int numCPU){
    return &TSSdescriptors[numCPU];
}