#include "keyboarddriver.h"

Keyboard* GlobalKeyboard;

const char ScanCodeSet1[] = {0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s','d','f','g','h','j','k','l',';','"','`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' '};

//thanks for help, @borrrden

void Keyboard::Handle(uint8_t keycode) {
    // 0x3E > number of entries in scan code set!!
    if(keycode > /* 0x3E */ sizeof(ScanCodeSet1)) return; //caps
    if (keycode == 0x0E && bufferIndex > 0) {
        buffer[--bufferIndex] = 0;
    } else {
        char translated = ScanCodeSet1[keycode];
        if(translated) {
            buffer[bufferIndex++] = translated;
        }
     }
}



/*  //broken implementation
    if(keycode > 0x3B) return; //caps
    if (keycode == 0x0E && bufferIndex > 0) {
        bufferIndex--;
        buffer[bufferIndex] = 0;
    } else {
        buffer[bufferIndex] = ScanCodeSet1[keycode];
        bufferIndex++;
    }
*/