#include <drivers/keyboard/keyboard.h>

Keyboard* GlobalKeyboard;

//thanks for help, @borrrden

char Keyboard::Translate(uint8_t keycode,KeyState isUpperCase,KeyState isCaps)
{
    // 0x3E > number of entries in scan code set!!
    if(keycode > /* 0x3E */ sizeof(ScanCodeSet1)-1) return 0;
    if(isUpperCase == DOWN && keycode != KEY_SPACE)
        return ScanCodeUpperSet1[keycode];
    else if(isCaps == DOWN && keycode != KEY_SPACE)
        return ScanCodeCapsSet1[keycode]; //fixed caps and shift to print random symbols
    else if(isUpperCase == DOWN && isCaps)
        return ScanCodeSet1[keycode];
    else
        return ScanCodeSet1[keycode];
}

void Keyboard::Handle(uint8_t keycode)
{
    ReceivedIntrerupt = true;
    LastKeycode = (int)Translate(keycode,(KeyState)((int)LeftShift | (int)RightShift),CapsLock);
    switch (keycode)
    {
    case KEY_BACKSPACE:
        if(bufferIndex)
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
        char translated = Translate(keycode,(KeyState)((int)LeftShift | (int)RightShift),CapsLock);
        if(translated)
        {
            if(bufferIndex+1 >= 0xFFFF) bufferIndex = 0; //prevent buffer overflow
            buffer[bufferIndex++] = translated;
        }
        break;
    }
}

int kbhit()
{
    if(ReceivedIntrerupt)
    {
        ReceivedIntrerupt = false;
        return 1;
    }

    return 0;
}

int getch()
{
    return LastKeycode;
}
