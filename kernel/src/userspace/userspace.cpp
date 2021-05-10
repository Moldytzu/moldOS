#include "userspace.h"
#include "../libc/stdio.h"
#include "../drivers/display/displaydriver.h"
#include "../io/serial.h"

void ConsoleWrite(char* text) {
    printf(text);
    GlobalDisplay->update();
}

void SerialWrite(char* text) {
    GlobalCOM1->Write(text);
}

void Exit(uint64_t code) {
    printf("Exiting userspace with exit code %u",code);
    GlobalDisplay->update();
    while (1);
}

void SyscallHandler(int syscall, int arg1, int arg2, int doNotModify, int arg3) {
    switch (syscall)
    {
    case SYSCALL_WRITE:
        ConsoleWrite((char*)(uint64_t*)arg1);
        break;
    case SYSCALL_SERIALWRITE:
        SerialWrite((char*)(uint64_t*)arg1);
        break;
    case SYSCALL_EXIT:
        Exit(arg1);
        break;

    default:
        break;
    }
}