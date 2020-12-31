#pragma once
#include <stdint.h>
#include "../../io/ports.h"
class RealTimeClock {
    public:
        void waitSeconds(uint32_t secs);
        uint32_t readHours();
        uint32_t readMinutes();
        uint32_t readSecond();
        uint32_t readTime();
};