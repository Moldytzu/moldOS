#include <memory/ptm.h>

PageTableManager GlobalTableManager = NULL;

PageTableManager::PageTableManager(PageTable *PML4Address)
{
    this->PML4 = PML4Address;
    this->PhysicalMemoryVirtualAddress = 0;
    this->VirtualAddress = 0x10000000000000000; //It must be a multiple of 0x1000
}

void *PageTableManager::GetPhysicalAddress(void *virtualAddress)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualAddress);
    PageDirectoryEntry PDE;

    PageTable *PML4VirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PML4);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    PageTable *PDP;
    PageTable *PDPVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        return 0;
    }
    else
    {
        PDP = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
        PDPVirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PDP);
    }

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    PageTable *PD;
    PageTable *PDVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        return 0;
    }
    else
    {
        PD = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
        PDVirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PD);
    }

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    PageTable *PT;
    PageTable *PTVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        return 0;
    }
    else
    {
        PT = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
        PTVirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PT);
    }

    PDE = PTVirtualAddress->entries[indexer.P_i];
    return (void *)((uint64_t)PDE.GetAddress() * 0x1000 + (uint64_t)virtualAddress % 0x1000);
}
void *PageTableManager::GetVirtualAddress(void *physicalAddress)
{
    return (void *)((uint64_t)PhysicalMemoryVirtualAddress + (uint64_t)physicalAddress);
}

void PageTableManager::GetIndexes(void *address, PageLevelIndexes *out)
{
    uint64_t addr = (uint64_t)address;
    addr >>= 12;
    out->L1_i = addr & 0x01FF;
    addr >>= 9;
    out->L2_i = addr & 0x01FF;
    addr >>= 9;
    out->L3_i = addr & 0x01FF;
    addr >>= 9;
    out->L4_i = addr & 0x01FF;
}

void PageTableManager::DefinePhysicalMemoryLocation(void *PhysicalMemoryVirtualAddress)
{
    uint64_t pmva = (uint64_t)PhysicalMemoryVirtualAddress;
    if ((uint64_t)PhysicalMemoryVirtualAddress % 0x1000)
    {
        pmva -= (uint64_t)PhysicalMemoryVirtualAddress % 0x1000;
        pmva += 0x1000;
    }
    this->PhysicalMemoryVirtualAddressSaver = (void*)pmva;
}

void PageTableManager::DefineVirtualTableLocation()
{
    this->PhysicalMemoryVirtualAddress = PhysicalMemoryVirtualAddressSaver;
    GlobalAllocator.PageBitmap.Buffer = (uint8_t *)GlobalTableManager.GetVirtualAddress(GlobalAllocator.PageBitmap.Buffer);
}

void PageTableManager::MapUserspaceMemory(void *virtualMemory)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PageTable *PML4VirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PML4);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    PageTable *PDP;
    PageTable *PDPVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        return;
    }
    else
    {
        PDP = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
        PDPVirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PDP);
    }

    PDE.SetFlag(PT_Flag::UserSuper, true);
    PML4VirtualAddress->entries[indexer.PDP_i] = PDE;

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    PageTable *PD;
    PageTable *PDVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        return;
    }
    else
    {
        PD = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
        PDVirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PD);
    }

    PDE.SetFlag(PT_Flag::UserSuper, true);
    PDPVirtualAddress->entries[indexer.PD_i] = PDE;

    PDE = PDVirtualAddress->entries[indexer.PT_i];

    PageTable *PT;
    PageTable *PTVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        return;
    }
    else
    {
        PT = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
        PTVirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PT);
    }

    PDE.SetFlag(PT_Flag::UserSuper, true);
    PDVirtualAddress->entries[indexer.PT_i] = PDE;

    PDE = PTVirtualAddress->entries[indexer.P_i];
    PDE.SetFlag(PT_Flag::UserSuper, true);
    PTVirtualAddress->entries[indexer.P_i] = PDE;
}

void PageTableManager::MapMemory(void *virtualMemory, void *physicalMemory)
{
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PageTable *PML4VirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PML4);
    PDE = PML4VirtualAddress->entries[indexer.PDP_i];
    PageTable *PDP;
    PageTable *PDPVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PDP = (PageTable *)GlobalAllocator.RequestPage();
        PDPVirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PDP);
        memset(PDPVirtualAddress, 0, 0x1000);
        PDE.SetAddress((uint64_t)PDP >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PML4VirtualAddress->entries[indexer.PDP_i] = PDE;
    }
    else
    {
        PDP = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
        PDPVirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PDP);
    }

    PDE = PDPVirtualAddress->entries[indexer.PD_i];

    PageTable *PD;
    PageTable *PDVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PD = (PageTable *)GlobalAllocator.RequestPage();
        PDVirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PD);
        memset(PDVirtualAddress, 0, 0x1000);
        PDE.SetAddress((uint64_t)PD >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDPVirtualAddress->entries[indexer.PD_i] = PDE;
    }
    else
    {
        PD = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
        PDVirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PD);
    }

    PDE = PDVirtualAddress->entries[indexer.PT_i];
    PageTable *PT;
    PageTable *PTVirtualAddress;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
        PT = (PageTable *)GlobalAllocator.RequestPage();
        PTVirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PT);
        memset(PTVirtualAddress, 0, 0x1000);
        PDE.SetAddress((uint64_t)PT >> 12);
        PDE.SetFlag(PT_Flag::Present, true);
        PDE.SetFlag(PT_Flag::ReadWrite, true);
        PDVirtualAddress->entries[indexer.PT_i] = PDE;
    }
    else
    {
        PT = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
        PTVirtualAddress = (PageTable *)GlobalTableManager.GetVirtualAddress(PT);
    }

    PDE = PTVirtualAddress->entries[indexer.P_i];
    PDE.SetAddress((uint64_t)physicalMemory >> 12);
    PDE.SetFlag(PT_Flag::Present, true);
    PDE.SetFlag(PT_Flag::ReadWrite, true);
    PTVirtualAddress->entries[indexer.P_i] = PDE;
}