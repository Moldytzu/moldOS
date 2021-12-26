#include <terminal/ansi.h>

void ANSIPrint(const char* text)
{
    //GlobalDisplay->puts(text);
    int index = -1;
    while(index < strlen(text))
    {
        index++;
        if(text[index] == '\033') //escape
        {
            GlobalDisplay->puts("ANSI escape sequence!\n");
            continue;
        }
        GlobalDisplay->putc(text[index]);
    }
}