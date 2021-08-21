#pragma once
#include "drivers/display/displaydriver.h"
#include "io/serial.h"
#include "misc/logging/log.h"

struct IntreruptFrame
{
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t sp;
    uint32_t ss;
};

void KernelPanic(const char* Message);