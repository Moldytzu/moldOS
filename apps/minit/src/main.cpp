#include <stdlib.h>

extern "C" void moldMain() {
    puts("Welcome to moldOS!\n");
    puts("By Moldu' (Nov. 2020 - Oct. 2021)\n\n");

    puts("Kernel vtty output:\n");
    puts(sys_getterminaloutput(0));

    puts("\nProcess' terminal no': ");
    putchar(sys_getprocessterminal() + '0');
    putchar('\n');

    puts("\nContents of /sample.text: \n");
    const char* buffer = (const char*)malloc(4096);
    fread((void*)buffer,43,1,fopen("/sample.text","r"));
    puts(buffer);

    exit(2021);
}