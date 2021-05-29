#include "loader.h"
#include <stddef.h>
#include "../libc/stdio.h"
#include "../memory/PageTableManager.h"

void* LoadELFExecutable(LLFSHeader* fs,const char* file) {
    LLFSEntry* entry = LLFSOpenFile(fs,file);
    if((void*)file == (void*)0) {
        return (void*)1;
    }

    void* Contents = LLFSReadFile(entry);
    if((void*)Contents == (void*)0) {
        return (void*)2;
    }

    Elf64_Ehdr* header = (Elf64_Ehdr*)Contents;


    //don't use it
    //doesn't work :/

    return (void*)(Contents+header->e_entry+sizeof(header));
    //return (void*)header->e_entry;
}