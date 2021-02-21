#pragma once
#include <stdint.h>
#include "../../misc/power/acpi.h"

struct PCIDevice {
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t Command;
    uint16_t Status;
    uint8_t RevisionID;
    uint8_t ProgramInterface;
    uint8_t Subclass;
    uint8_t Class;
    uint8_t CacheLineSize;
    uint8_t LatencyTimer;
    uint8_t HeaderType;
    uint8_t BIST;
};

class PCI {
private:
    void EnumFunc(uint64_t addr,uint64_t function);
    void EnumDevice(uint64_t addr, uint64_t device);
    void EnumBus(uint64_t addr, uint64_t bus);
public:
    PCIDevice* Devices[0x2000];
    int DevicesIndex = 0;
    void EnumeratePCI(MCFG* mcfg);
};