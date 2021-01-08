#pragma once
#include <stdint.h>
#include "../../io/ports.h"
class RealTimeClock {
    private:
        unsigned char getRegister(int reg);
        int getUpdateInProgress();
    public:
        void waitSeconds(uint32_t secs);
        uint32_t readHours();
        uint32_t readMinutes();
        uint32_t readSeconds();
        uint32_t readTime();
};