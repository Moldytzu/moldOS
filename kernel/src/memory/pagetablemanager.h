#pragma once
#include "paging.h"
#include "pagemapindexer.h"
#include "stdint.h"
#include "pagefileallocator.h"
#include "efiMemory.h"

class PageTableManager
{
public:
    PageTableManager(PageTable* PML4Address);
    PageTable* PML4;
    void MapMemory(void* virtualMemory, void* physicalMemory);
};
