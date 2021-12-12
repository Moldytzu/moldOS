#include <stdlib.h>

extern "C" void moldMain() {
    puts("Welcome to moldOS!\n");
    puts("By Moldu' (Nov. 2020 - Oct. 2021)\n\n");

    puts("Kernel vtty output:\n");
    puts(sys_getterminaloutput(0));

    puts("\nProcess' terminal no': ");
    putchar(sys_getprocessterminal() + '0');
    putchar('\n');
    exit(2021);
}