#pragma once
#include <stdint.h>

extern "C" void EnableSCE();
extern "C" void RunInUserspace(void* function,void* stack); 

void SYSHandle(uint64_t a,uint64_t b,uint64_t c);
extern "C" void UserAPP();