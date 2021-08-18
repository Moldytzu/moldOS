#pragma once
#include "../io/ports.h"
#include "../drivers/keyboard/keyboarddriver.h"
#include "../io/serial.h"
#include "../panic.h"
#include "../drivers/display/displaydriver.h"
#include "../drivers/mouse/mouse.h"
#include "../scheduling/pit.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

struct InterruptStack {
    uint64_t rax; uint64_t rbx; uint64_t rcx; uint64_t rdx; uint64_t KernelRsp; uint64_t rsi; uint64_t rdi; uint64_t rbp;
    uint64_t r8; uint64_t r9; uint64_t r10; uint64_t r11; uint64_t r12; uint64_t r13; uint64_t r14; uint64_t r15;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
}__attribute__((packed));

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