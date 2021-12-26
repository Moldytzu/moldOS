#pragma once
#include <io/ports.h>
#include <drivers/keyboard/keyboard.h>
#include <io/serial.h>
#include <panic.h>
#include <drivers/display/display.h>
#include <drivers/mouse/mouse.h>
#include <scheduling/pit.h>
#include <scheduling/taskmgr.h>
#include <userspace/userspace.h>
#include <terminal/terminal.h>
#include <terminal/ansi.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

#define IDT_TA_InterruptGate    0b10001110
#define IDT_TA_CallGate         0b10001100
#define IDT_TA_TrapGate         0b10001111

struct IDTDescriptorEntry
{
    uint16_t Offset0;
    uint16_t Selector;
    uint8_t IST;
    uint8_t Type_Attributes;
    uint16_t Offset1;
    uint32_t Offset2;
    uint32_t Ignore;
    void setOffset(uint64_t Offset);
    uint64_t getOffset();
};

struct IDTR
{
    uint16_t Limit;
    uint64_t Offset;
} __attribute__((packed));

extern "C" void InvalideOpcodeHandler();//6
extern "C" void GeneralProtectionFaultHandler();//13
extern "C" void PageFaultHandler(); //14
extern "C" void DoubleFaultHandler();
extern "C" void KBHandler();
extern "C" void MSHandler();
extern "C" void PITHandler(InterruptStack* istack);

extern "C" void InvalideOpcodeHandlerEntry();//6
extern "C" void GeneralProtectionFaultHandlerEntry();//13
extern "C" void PageFaultHandlerEntry(); //14
extern "C" void DoubleFaultHandlerEntry();
extern "C" void KBHandlerEntry();
extern "C" void MSHandlerEntry();
extern "C" void PITHandlerEntry();

void RemapPIC();
void PIC_EndMaster();
void PIC_EndSlave();

void CreateIntrerupt(void* handler,uint8_t offset,uint8_t typeAttributes,uint8_t selector);

extern IDTR* idt;