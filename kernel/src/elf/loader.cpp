#include "loader.h"
#include <stddef.h>
#include "../libc/stdio.h"
#include "../memory/PageTableManager.h"
#include "../memory/heap.h"

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
    Elf64_Phdr *phdrs = (Elf64_Phdr *)(Contents+header->e_phoff);

	for (Elf64_Phdr *phdr = phdrs; (char *)phdr < (char *)phdrs + header->e_phnum * header->e_phentsize; phdr = (Elf64_Phdr *)((char *)phdr + header->e_phentsize)) {
		switch (phdr->p_type) {
		case PT_LOAD: {
				Elf64_Addr segment = (Elf64_Addr)Contents+phdr->p_paddr;
				uint64_t size = phdr->p_filesz;
                memcpy((void*)segment,Contents+phdr->p_offset,size);
                GlobalTableManager.MapUserspaceMemory((void*)segment);
				break;
			}
		}
	}

    //don't use it
    //doesn't work :/

    int (*Entry)() = ((__attribute__((sysv_abi)) int (*)())(void*)Contents+header->e_entry);

    Entry();

    while(1);
    return (void*)(header->e_entry);
    //return (void*)header->e_entry;
}