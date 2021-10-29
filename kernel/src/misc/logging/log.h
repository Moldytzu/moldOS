#pragma once
#include <drivers/display/display.h>
#include <misc/colors.h>
#include <io/serial.h>
#include <settings.h>
#include <drivers/rtc/rtc.h>

void LogInfo(const char* text);
void LogWarn(const char* text);
void LogError(const char* text);