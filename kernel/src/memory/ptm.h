#pragma once
#include <memory/paging.h>
#include <memory/pmi.h>
#include <stdint.h>
#include <memory/pfa.h>
#include <memory/memory.h>

// x86_64 multi-level addressing
struct PageLevelIndexes
{
    uint16_t	L4_i;	// Index to the page directory pointer table
    uint16_t	L3_i;	// Index to the page directory table
    uint16_t	L2_i;	// Index to the page table
    uint16_t	L1_i;	// Index to the page
};

class PageTableManager
{
public:
    PageTableManager(PageTable* PML4Address);

    void GetIndexes(void* address, PageLevelIndexes* out);
    void MapMemory(void* virtualMemory, void* physicalMemory);
    void MapUserspaceMemory(void* virtualMemory);
    void* GetPhysicalAddress(void* virtualAddress);
    void* GetVirtualAddress(void* physicalAddress); 
    void DefinePhysicalMemoryLocation(void* PhysicalMemoryVirtualAddress);
    void DefineVirtualTableLocation();
      PageTable* PML4;
        uint64_t VirtualAddress;  
    void* PhysicalMemoryVirtualAddressSaver;
    void* PhysicalMemoryVirtualAddress;
};

extern PageTableManager GlobalTableManager;