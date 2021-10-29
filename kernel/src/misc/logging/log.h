#pragma once
#include <drivers/display/display.h>
#include <misc/colors.h>
#include <io/serial.h>

void LogInfo(const char* text);
void LogWarn(const char* text);
void LogError(const char* text);