#pragma once
#include <stdint.h>

//standard
char* inttostr(uint64_t val);
char* inttostr(uint32_t val);
char* inttostr(uint16_t val);
char* inttostr(uint8_t val);
char* inttostr(int val);
char* inttostr(int64_t val);

//float
char* inttostr(double val,uint8_t decimalPlaces);
char* inttostr(double val);

//hex
char* inttohstr(uint64_t val);
char* inttohstr(uint32_t val);
char* inttohstr(uint16_t val);
char* inttohstr(uint8_t val);