#pragma once
#include <stdint.h>

#define MSR_CPU_TEMP 0x19C
#define MSR_CPU_TJUNCTION 0x1A2

void ReadMSR(uint32_t msr,uint32_t* low,uint32_t* high);
uint32_t GetCPUTJunction();
uint32_t GetCPUTemp();