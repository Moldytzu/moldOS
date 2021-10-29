#include <io/msr.h>

void ReadMSR(uint32_t msr,uint32_t* low,uint32_t* high) {
    asm volatile("rdmsr" : "=a"(*low), "=d"(*high) : "c"(msr));
}

uint32_t GetCPUTJunction() {
    uint32_t TJunctionLow;
	uint32_t TJunctionHigh;
    ReadMSR(MSR_CPU_TJUNCTION,&TJunctionLow,&TJunctionHigh);
    return (TJunctionLow >> 16) & 0xff;
}

uint32_t GetCPUTemp() {
    uint32_t TempLow;
	uint32_t TempHigh;

	ReadMSR(MSR_CPU_TEMP,&TempLow,&TempHigh);
    return GetCPUTJunction()-((TempLow >> 16) & 0x7f); //the proper way
}