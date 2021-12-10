#include <stdlib.h>

extern "C" void moldMain() {
    puts("Welcome to moldOS!\n");
    puts("By Moldu' (Nov. 2020 - Oct. 2021)\n");
    puts(sys_getterminaloutput(0));
    exit(2021);
}