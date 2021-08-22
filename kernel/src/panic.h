#pragma once
#include "drivers/display/displaydriver.h"
#include "io/serial.h"
#include "misc/logging/log.h"
#include "scheduling/taskmgr.h"

void KernelPanic(const char* Message);