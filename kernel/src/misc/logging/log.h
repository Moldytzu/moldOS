#pragma once
#include "../../drivers/display/displaydriver.h"
#include "../colors.h"
#include "../../io/serial.h"

class Logging {
public:
    void info(const char* text);
    void warn(const char* text);
    void error(const char* text);
};