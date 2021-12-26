#include <terminal/ansi.h>

void handle(int arg)
{
    switch(arg)
    {
    case 1: //bold
    case 2: //non-bold
    case 4: //underline
    case 5: //slow blink
        break; //unsupported
    case 30: //fg
        GlobalDisplay->colour = BLACK;
        break;
    case 31:
        GlobalDisplay->colour = LIGHTRED;
        break;
    case 32:
        GlobalDisplay->colour = LIGHTGREEN;
        break;
    case 33:
        GlobalDisplay->colour = YELLOW;
        break;
    case 34:
        GlobalDisplay->colour = LIGHTBLUE;
        break;
    case 35:
        GlobalDisplay->colour = LIGHTMAGENTA;
        break;
    case 36:
        GlobalDisplay->colour = LIGHTTURQOISE;
        break;
    case 37:
        GlobalDisplay->colour = WHITE;
        break;
    default: //reset
        GlobalDisplay->colour = WHITE;
        break;
    }
}

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
                char param1[3];
                char param2[3];

                //clear them
                memset(param1,0,3);
                memset(param2,0,3);

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

                handle(atoi(param1));
                handle(atoi(param2));
            }
            continue;
        }
        GlobalDisplay->putc(text[index]);
    }
}