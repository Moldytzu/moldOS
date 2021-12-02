#include <libc/stdio/printf.h>

void printf(const char* str, ...) {
    va_list args;
    va_start(args,str);
    int index = 0;
    while(str[index] != 0) {
        if(str[index] == '%') {
            if(str[index+1] == 'd')
                VirtualTerminals[0].append(inttostr(va_arg (args, int))); //int or decimal
            else if(str[index+1] == 'u')
                VirtualTerminals[0].append(inttostr(va_arg (args, uint64_t))); //uint
            else if(str[index+1] == 'x')
                VirtualTerminals[0].append(inttohstr(va_arg (args, uint64_t))); 
            else if(str[index+1] == 'f')
                VirtualTerminals[0].append(inttostr(va_arg (args, double))); //float
            else if(str[index+1] == 'c' && str[index+2] != 'o')
                VirtualTerminals[0].append(chartostr(va_arg (args, int))); //char
            else if(str[index+1] == 's')
                VirtualTerminals[0].append(va_arg (args, const char*)); //string
            else if(str[index+1] == 'p')
                VirtualTerminals[0].append(inttostr((uint64_t)va_arg (args, void*))); //address
            else if(str[index+1] == 'c' && str[index+2] == 'o') {
                GlobalDisplay->colour = va_arg (args, int); //colour
                index++;
            }
            index++;
        } else VirtualTerminals[0].append(chartostr(str[index])); //char
        index++;
    }
    va_end(args);
}