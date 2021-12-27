#include <stdlib.h>

extern "C" void moldMain() {
    puts("Welcome to moldOS!\n");
    puts("By Moldu' (Nov. 2020 - Jan. 2022)\n\n");

    puts("\033[1;30mCOLOUR!\n");
    puts("\033[1;31mCOLOUR!\n");
    puts("\033[1;32mCOLOUR!\n");
    puts("\033[1;33mCOLOUR!\n");
    puts("\033[1;34mCOLOUR!\n");
    puts("\033[1;35mCOLOUR!\n");
    puts("\033[1;36mCOLOUR!\n");
    puts("\033[1;37mCOLOUR!\n");

    puts("\nProcess' terminal no': ");
    putchar(sys_getprocessterminal() + '0');
    putchar('\n');

    puts("\nProcess' pid: ");
    putchar(sys_getpid() + '0');
    putchar('\n');

    puts("\n'/minit.melf' task's pid: ");
    putchar(sys_getpidbyname("/minit.melf") + '0');
    putchar('\n');

    puts("\nIdle task's pid: ");
    putchar(sys_getpidbyname("Idle Task") + '0');
    putchar('\n');

    puts("\nContents of /sample.text: \n");
    const char* buffer = (const char*)malloc(4096);
    fread((void*)buffer,43,1,fopen("/sample.text","r"));
    puts(buffer);

    exit(2021);
}