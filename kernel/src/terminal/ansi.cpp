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
            if(index+1 < strlen(text) && text[index+1] == '[')
            {
                index+=2;
                //loop thru until an 'm' appears
                do
                {
                    GlobalDisplay->putc(text[index]);
                    index++;
                }
                while(text[index] != 'm');
            }
            continue;
        }
        GlobalDisplay->putc(text[index]);
    }
}