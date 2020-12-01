#pragma once
#include <stdint.h>

char* inttostr(uint64_t val);
char* inttostr(int64_t val);
char* inttostr(double val,uint8_t decimalPlaces);
char* inttostr(double val);

char* inttohstr(uint64_t val);
char* inttohstr(uint32_t val);
char* inttohstr(uint16_t val);
char* inttohstr(uint8_t val);