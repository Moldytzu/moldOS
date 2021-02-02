#include "keyboarddriver.h"

Keyboard* GlobalKeyboard;

const char ScanCodeSet1[] = {0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s','d','f','g','h','j','k','l',';','"','`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' '};

//thanks for help, @borrrden

char Keyboard::Translate(uint8_t keycode,KeyState isUpperCase) {
    // 0x3E > number of entries in scan code set!!
    if(keycode > /* 0x3E */ sizeof(ScanCodeSet1)-1) return 0; 
    if(isUpperCase == DOWN && keycode != KEY_SPACE)
        return ScanCodeSet1[keycode] - ASCII_UPPERCASE_OFFSET;
    else 
        return ScanCodeSet1[keycode];
}

void Keyboard::Handle(uint8_t keycode) {
    switch (keycode)
    {
        case KEY_BACKSPACE:
            if(bufferIndex > 0)
                buffer[--bufferIndex] = 0;
            break;
        case KEY_LEFTSHIFT:
            LeftShift = DOWN;
            break;
        case KEY_RIGHTSHIFT:
            RightShift = DOWN;
            break;
        case KEY_LEFTSHIFT + KEYUP_OFFSET:
            LeftShift = UP;
            break;
        case KEY_RIGHTSHIFT + KEYUP_OFFSET:
            RightShift = UP;
            break;
        case KEY_CAPSLOCK:
            if(CapsLock == UP)
                CapsLock = DOWN;
            else
                CapsLock = UP;
            break;
        default:
            char translated = Translate(keycode,(KeyState)((int)LeftShift | (int)RightShift | (int)CapsLock));
            if(translated)
                buffer[bufferIndex++] = translated;
            break;
    }
}