#include "msr.h"

void ReadMSR(uint32_t msr,uint32_t* low,uint32_t* high) {
    asm volatile("rdmsr" : "=a"(*low), "=d"(*high) : "c"(msr));
}

uint32_t GetCPUTemp() {
    uint32_t TempLow;
	uint32_t TempHigh;

	ReadMSR(MSR_CPU_TEMP,&TempLow,&TempHigh);
    return ((TempLow >> 16) & 0x7f) - 30;
}