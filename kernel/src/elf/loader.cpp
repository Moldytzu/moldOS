#include <elf/loader.h>

int CheckELF(Elf64_Ehdr* header)
{
    if (header->e_ident[EI_CLASS] != ELFCLASS64 || header->e_ident[EI_DATA] != ELFDATA2LSB || header->e_type != ET_EXEC || header->e_machine != EM_X86_64 || header->e_version != EV_CURRENT)
    {
        return 0;
    }
    return 1;
}

void* LoadELFExecutable(const char* file, bool pie)
{
    FileDescriptor* fDescriptor = VFSOpenFile(file);
    void* Contents = VFSReadFile(fDescriptor);

    if((void*)Contents == (void*)0)
        return Contents;

    Elf64_Ehdr* header = (Elf64_Ehdr*)Contents;
    Elf64_Phdr *phdrs = (Elf64_Phdr *)((uint64_t)Contents+header->e_phoff);

    if(!CheckELF(header)) return (void*)2;

    void* offset = GlobalAllocator.RequestPages(VFSSizeFile(fDescriptor)/4096+1); //allocate ram for the elf

    for (uint64_t i = 0; i < header->e_phnum; i++)
    {
        if(phdrs->p_type == PT_LOAD)
        {
            uint64_t segmentStart = ((uint64_t)offset+phdrs->p_offset); //get the segment start
            printf("offset: %x\n",segmentStart);
            memcpy((void*)segmentStart,(void*)((uint64_t)Contents+phdrs->p_offset),phdrs->p_filesz);
        }
        phdrs++;
    }

    return (void*)((uint64_t)offset+header->e_entry-0xFFFF800000000000);
}