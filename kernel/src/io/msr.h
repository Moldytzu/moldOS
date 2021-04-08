#pragma once
#include <stdint.h>

#define MSR_CPU_TEMP 0x19C

void ReadMSR(uint32_t msr,uint32_t* low,uint32_t* high);
uint32_t GetCPUTemp();