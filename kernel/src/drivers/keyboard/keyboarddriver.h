#pragma once
#include <stdint.h>
#include "../display/displaydriver.h"
#include "../../io/ports.h"
class Keyboard {
    public:
        char buffer[0xFFFF];
        int bufferIndex = 0;
        void Handle(uint8_t keycode);
};

extern Keyboard* GlobalKeyboard;