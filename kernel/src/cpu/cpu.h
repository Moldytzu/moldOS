#pragma once
#include <stdint.h>

inline void cpuid(uint32_t reg, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
char* cpuGetName();
char* cpuGetVendor();