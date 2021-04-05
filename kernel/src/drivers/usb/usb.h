#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../../libc/stdio.h"
#include "../../memory/PageTableManager.h"
#include "../../memory/heap.h"
#include "../pci/pcidefs.h"
#include "../../io/ports.h"
#include "../../scheduling/pit.h"
#include "../../memory/PageFrameAllocator.h"

#define CMD_MAX_PACKET 1 << 7
#define CMD_CONFIG_DONE 1 << 6
#define CMD_SOFT_DEBUG 1 << 5
#define CMD_GLOBAL_RESUME 1 << 4
#define CMD_GLOBAL_SUSPEND 1 << 3
#define CMD_GLOBAL_RESET 1 << 2
#define CMD_HOST_CTRL_RESET 1 << 1
#define CMD_RUN_STOP 1 << 0

struct UHCI_REGISTERS {
    uint16_t USBCMD;
    uint16_t USBSTS;
    uint16_t USBINTR;
    uint16_t FRNUM;
    uint32_t FRBASEADD;
    uint8_t SOFMOD;
    uint16_t PORTSC1;
    uint16_t PORTSC2;
}__attribute__((packed));

void UHCIInit(PCIDevice* device);