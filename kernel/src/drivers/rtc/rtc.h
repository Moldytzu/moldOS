#pragma once
#include <stdint.h>
#include <io/ports.h>

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

unsigned char RTCgetRegister(int reg);
int RTCgetUpdateInProgress();
void RTCwaitUpdate();
void RTCwaitSeconds(uint32_t secs);
uint32_t RTCreadHours();
uint32_t RTCreadMinutes();
uint32_t RTCreadSeconds();
uint32_t RTCreadTime();
uint32_t RTCreadDay();
uint32_t RTCreadMonth();
uint32_t RTCreadYear();
uint32_t BCDToBinary(uint32_t bcd);