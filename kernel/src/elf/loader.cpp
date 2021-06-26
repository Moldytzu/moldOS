#include "loader.h"
#include <stddef.h>
#include "../libc/stdio.h"
#include "../memory/PageTableManager.h"
#include "../memory/heap.h"

void* LoadELFExecutable(LLFSHeader* fs,const char* file) {
    LLFSEntry* entry = LLFSOpenFile(fs,file);
    
    if((void*)entry == (void*)0) {
        return (void*)1;
    }

    void* Contents = LLFSReadFile(entry);

    Elf64_Ehdr* header = (Elf64_Ehdr*)Contents;
    Elf64_Phdr *phdrs = (Elf64_Phdr *)(Contents+header->e_phoff);

    void* offset = malloc(entry->FileSize); //allocate ram for the elf

    for (uint64_t i = 0;i < header->e_phnum;i++) {
        if(phdrs->p_type == PT_LOAD) {
            void* segmentStart = offset+phdrs->p_vaddr; //get the segment start
            memcpy(segmentStart,Contents + phdrs->p_offset,phdrs->p_filesz); 
        }
        phdrs++;
    }

    return (void*)offset+header->e_entry;
    //return (void*)header->e_entry;
}