#pragma once
#include <stdint.h>
#include "../display/displaydriver.h"
#include "../../io/ports.h"

#define KEY_BACKSPACE 0x0E
#define KEY_LEFTSHIFT 0x2A
#define KEY_RIGHTSHIFT 0x36
#define KEY_CAPSLOCK 0x3A
#define KEY_SPACE 0x39
#define KEY_ALT 0x38
#define KEY_CTRL 0x1E

#define KEYUP_OFFSET 0x80

#define ASCII_UPPERCASE_OFFSET 32

enum KeyState {
    UP,
    DOWN
};

class Keyboard {
    public:
        char buffer[0xFFFF];
        int bufferIndex = 0;
        
        KeyState LeftShift = UP;
        KeyState RightShift = UP;
        KeyState CapsLock = UP;
        
        char Translate(uint8_t keycode,KeyState isUpperCase);
        void Handle(uint8_t keycode);
};

extern Keyboard* GlobalKeyboard;