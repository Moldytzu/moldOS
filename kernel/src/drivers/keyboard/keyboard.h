#pragma once
#include <stdint.h>
#include <drivers/display/display.h>
#include <io/ports.h>

#define KEY_BACKSPACE 0x0E
#define KEY_LEFTSHIFT 0x2A
#define KEY_RIGHTSHIFT 0x36
#define KEY_CAPSLOCK 0x3A
#define KEY_SPACE 0x39
#define KEY_ALT 0x38
#define KEY_CTRL 0x1E

#define KEYUP_OFFSET 0x80

#define ASCII_UPPERCASE_OFFSET 0x20

static bool ReceivedIntrerupt = false;
static int LastKeycode = 0;

const char ScanCodeSet1[] = {0,0,'1','2','3','4','5','6','7','8','9','0','-','=','\b',0,'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' '};
const char ScanCodeUpperSet1[] = {0,0,'1','@','#','$','%','^','&','*','(',')','_','+','\b',0,'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,'A','S','D','F','G','H','J','K','L',':','"','~',0,'|','Z','X','C','V','B','N','M','<','>','?',0,'*',0,' '};
const char ScanCodeCapsSet1[] = {0,0,'1','2','3','4','5','6','7','8','9','0','-','=','\b',0,'Q','W','E','R','T','Y','U','I','O','P','[',']','\n',0,'A','S','D','F','G','H','J','K','L',';','\'','`',0,'\\','Z','X','C','V','B','N','M',',','.','/',0,'*',0,' '};

enum KeyState
{
    UP,
    DOWN
};

class Keyboard
{
public:
    char buffer[0xFFFF];
    int bufferIndex = 0;

    KeyState LeftShift = UP;
    KeyState RightShift = UP;
    KeyState CapsLock = UP;

    char Translate(uint8_t keycode,KeyState isUpperCase,KeyState isCaps);
    void Handle(uint8_t keycode);

};

int kbhit();
int getch();

extern Keyboard* GlobalKeyboard;