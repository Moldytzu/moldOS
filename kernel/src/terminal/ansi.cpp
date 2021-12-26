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

                //buffers to store the parameters individualy
                char param1[2];
                char param2[2];

                //clear them
                memset(param1,0,2);
                memset(param2,0,2);

                bool switched = false;
                int paramIndex = 0;
                //loop thru until an 'm' appears
                do
                {
                    if(text[index] == ';')
                    {
                        paramIndex = 0;
                        switched = true;
                    }
                    else
                    {
                        if(switched)
                            param2[paramIndex] = text[index];
                        else
                            param1[paramIndex] = text[index];
                        paramIndex++;
                    }
                    index++;
                }
                while(text[index] != 'm');

                GlobalDisplay->puts(" First param is ");
                GlobalDisplay->putc(param1[0]);
                GlobalDisplay->putc(param1[1]);
                GlobalDisplay->puts(" Second param is ");
                GlobalDisplay->putc(param2[0]);
                GlobalDisplay->putc(param2[1]);
                GlobalDisplay->putc(' ');
            }
            continue;
        }
        GlobalDisplay->putc(text[index]);
    }
}