#include "printf.h"

void printf(const char* str, ...) {
    va_list args;
    va_start(args,str);
    
    int index = 0;
    while(str[index] != 0) {
        if(str[index] == '%' && (str[index+1] == 'd' || str[index+1] == 'i')) {
            GlobalDisplay->puts(inttostr(va_arg (args, int)));
            index++;
        } else if (str[index] == '%' && str[index+1] == 'u') {
            GlobalDisplay->puts(inttostr(va_arg (args, uint64_t)));
            index++;
        } else if (str[index] == '%' && (str[index+1] == 'x' || str[index+1] == 'X')) {
            GlobalDisplay->puts(inttohstr(va_arg (args, uint64_t)));
            index++;    
        } else if (str[index] == '%' && (str[index+1] == 'f' || str[index+1] == 'F')) {
            GlobalDisplay->puts(inttostr(va_arg (args, double)));
            index++;
        } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] != 'o') {
            GlobalDisplay->puts(chartostr(va_arg (args, char)));
            index++;
        } else if (str[index] == '%' && str[index+1] == 's') {
            GlobalDisplay->puts(va_arg (args, char*));
            index++;
        } else if (str[index] == '%' && str[index+1] == 'p') {
            GlobalDisplay->puts(inttostr((uint64_t)va_arg (args, void*)));
            index++;
        } else if (str[index] == '%' && str[index+1] == 'p') {
            GlobalDisplay->puts(inttostr((uint64_t)va_arg (args, void*)));
            index++;
        } else if (str[index] == '%' && str[index+1] == 'c' && str[index+2] == 'o') {
            GlobalDisplay->colour = va_arg (args, int);
            index+=2;
        } else {
            GlobalDisplay->puts(chartostr(str[index]));
        }
        index++;
    }

    va_end(args);
}