#pragma once
#include "PageFrameAllocator.h"
#include <stdint.h>

void* malloc(size_t size);
void* calloc(size_t num,size_t size);
void free(void* adress);