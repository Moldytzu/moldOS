#include "loader.h"
#include <stddef.h>
#include "../libc/stdio.h"
#include "../memory/PageTableManager.h"
#include "../memory/heap.h"

int CheckELF(LLFSHeader* fs,const char* file) {
    LLFSEntry* entry = LLFSOpenFile(fs,file);

    if((void*)entry == (void*)0) {
        return 0;
    }

    Elf64_Ehdr* header = (Elf64_Ehdr*)LLFSReadFile(entry);
    if (header->e_ident[EI_CLASS] != ELFCLASS64 || header->e_ident[EI_DATA] != ELFDATA2LSB || header->e_type != ET_EXEC || header->e_machine != EM_X86_64 || header->e_version != EV_CURRENT) {
        return 0;
    }

    return 1;
}

void* LoadELFExecutable(LLFSHeader* fs,const char* file, bool pie) {
    LLFSEntry* entry = LLFSOpenFile(fs,file);

    if((void*)entry == (void*)0) {
        return (void*)1;
    }

    void* Contents = LLFSReadFile(entry);

    Elf64_Ehdr* header = (Elf64_Ehdr*)Contents;
    Elf64_Phdr *phdrs = (Elf64_Phdr *)(Contents+header->e_phoff);

    if(!CheckELF(fs,file)) return (void*)2;

    void* offset = malloc(entry->FileSize); //allocate ram for the elf

    for (uint64_t i = 0;i < header->e_phnum;i++) {
        if(phdrs->p_type == PT_LOAD) {
            void* segmentStart = offset+phdrs->p_vaddr; //get the segment start
            if(!pie) segmentStart -= (uint64_t)offset;
            memcpy(segmentStart,Contents + phdrs->p_offset,phdrs->p_filesz); 
        }
        phdrs++;
    }

    if(!pie) return (void*)header->e_entry;
    return (void*)offset+header->e_entry;
}