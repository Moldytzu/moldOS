#pragma once
#include <stdint.h>
#include <misc/power/acpi.h>
#include <drivers/pci/pcitranslate.h>
#include <memory/PageTableManager.h>
#include <drivers/display/display.h>
#include <libc/stdio.h>
#include <memory/heap.h>

class PCI {
private:
    void EnumFunc(uint64_t addr,uint64_t function);
    void EnumDevice(uint64_t addr, uint64_t device);
    void EnumBus(uint64_t addr, uint64_t bus);
public:
    TranslatedPCIDevice Devices[0x2000];
    int DevicesIndex = 0;
    void EnumeratePCI(MCFG* mcfg);
};