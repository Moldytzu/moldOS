#pragma once
#include "../../drivers/display/displaydriver.h"
#include "../colors.h"
#include "../../io/serial.h"

void LogInfo(const char* text);
void LogWarn(const char* text);
void LogError(const char* text);