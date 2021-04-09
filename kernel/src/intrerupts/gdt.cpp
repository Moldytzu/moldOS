#include "gdt.h"

__attribute__((aligned(0x1000)))
GDT DefaultGDT = {
    {0, 0, 0, 0x00, 0x00, 0},  /* 0x00 null  */
    {0, 0, 0, 0x9a, 0xa0, 0},  /* 0x08 kernel code (kernel base selector) */
    {0, 0, 0, 0x92, 0xa0, 0},  /* 0x10 kernel data */
    {0, 0, 0, 0x00, 0x00, 0},  /* 0x18 null (user base selector) */
    {0, 0, 0, 0x92, 0xa0, 0},  /* 0x20 user data */
    {0, 0, 0, 0x9a, 0xa0, 0},  /* 0x28 user code */
    {0, 0, 0, 0x92, 0xa0, 0},  /* 0x30 ovmf data */
    {0, 0, 0, 0x9a, 0xa0, 0},  /* 0x38 ovmf code */
    {0, 0, 0, 0x89, 0xa0, 0},  /* 0x40 tss low */
    {0, 0, 0, 0x00, 0x00, 0},  /* 0x48 tss high */
};

TSS* GlobalTSS;

void TSSInit(void* stack) {
    GlobalTSS = (TSS*)GlobalAllocator.RequestPage();
    memset(GlobalTSS,0,sizeof(GlobalTSS));
    uint64_t base = (uint64_t)GlobalTSS;
    DefaultGDT.TSSLow.Base0 = base & 0xFFFF;
    DefaultGDT.TSSLow.Base1 = (base >> 16) & 0xFF;
    DefaultGDT.TSSLow.Base2 = (base >> 24) & 0xFF;
    DefaultGDT.TSSLow.Limit0 = sizeof(TSS);
    DefaultGDT.TSSHigh.Limit0 = (base >> 32) & 0xFFFF;
    DefaultGDT.TSSHigh.Base0 = (base >> 48) & 0xFFFF;
}

void GDTSetKernelStack(uint64_t reserved,void* stack) {
    printf("GDTSETKERNELSTACK");GlobalDisplay->update();
    GlobalTSS->rsp0 = (uint64_t)stack;
    printf("GDTSETKERNELSTACK2");GlobalDisplay->update();
}