#include <libc/stdio/printf.h>

void printf(const char* str, ...)
{
    va_list args;
    va_start(args,str);
    printf(str,args);
    va_end(args);
}

void printf(const char* str, va_list list)
{
    int index = 0;
    while(str[index] != 0)
    {
        if(str[index] == '%')
        {
            if(str[index+1] == 'd')
                VirtualTerminals[CurrentTerminal].append(inttostr(va_arg (list, int))); //int or decimal
            else if(str[index+1] == 'u')
                VirtualTerminals[CurrentTerminal].append(inttostr(va_arg (list, uint64_t))); //uint
            else if(str[index+1] == 'x')
                VirtualTerminals[CurrentTerminal].append(inttohstr(va_arg (list, uint64_t)));
            else if(str[index+1] == 'f')
                VirtualTerminals[CurrentTerminal].append(inttostr(va_arg (list, double))); //float
            else if(str[index+1] == 'c' && str[index+2] != 'o')
                VirtualTerminals[CurrentTerminal].append(chartostr(va_arg (list, int))); //char
            else if(str[index+1] == 's')
                VirtualTerminals[CurrentTerminal].append(va_arg (list, const char*)); //string
            else if(str[index+1] == 'p')
                VirtualTerminals[CurrentTerminal].append(inttostr((uint64_t)va_arg (list, void*))); //address
            else if(str[index+1] == 'c' && str[index+2] == 'o')
            {
                va_arg (list, int);
                index++;
            }
            index++;
        }
        else VirtualTerminals[CurrentTerminal].append(chartostr(str[index]));   //char
        index++;
    }
}

void printf(uint32_t vt,const char* str, ...)
{
    uint32_t tmp = CurrentTerminal;
    CurrentTerminal = vt;
    va_list args;
    va_start(args,str);
    printf(str,args);
    va_end(args);
    CurrentTerminal = tmp;
}