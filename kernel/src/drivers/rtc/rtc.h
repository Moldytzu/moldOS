#pragma once
#include <stdint.h>
#include "../../io/ports.h"

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