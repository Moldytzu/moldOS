#include "cpu.h"

inline void cpuid(uint32_t reg, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
    __asm__ volatile("cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "0" (reg));
}

char vendor[128];
char* cpuGetVendor() {
    cpuid(0x80000002, (uint32_t *)(vendor +  0), (uint32_t *)(vendor +  4), (uint32_t *)(vendor +  8), (uint32_t *)(vendor + 12));
    cpuid(0x80000003, (uint32_t *)(vendor + 16), (uint32_t *)(vendor + 20), (uint32_t *)(vendor + 24), (uint32_t *)(vendor + 28));
    cpuid(0x80000004, (uint32_t *)(vendor + 32), (uint32_t *)(vendor + 36), (uint32_t *)(vendor + 40), (uint32_t *)(vendor + 44));
    vendor[127] = 0;
    return vendor;
}

char vendor2[13];
char* cpuGetName() {
    uint32_t standardfunc;
    cpuid(0, &standardfunc, (uint32_t *)(vendor2 + 0), (uint32_t *)(vendor2 + 8), (uint32_t *)(vendor2 + 4));
    vendor[12] = 0;
    return vendor2;
}