#pragma once
#include "../drivers/display/displaydriver.h"
#include "../io/serial.h"
#include "../misc/colors.h"
#include "../io/ports.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

struct IntreruptFrame;
__attribute__((interrupt)) void GeneralProtectionFaultHandler(struct IntreruptFrame* frame);//13
__attribute__((interrupt)) void PageFaultHandler(struct IntreruptFrame* frame); //14
__attribute__((interrupt)) void DoubleFaultHandler(struct IntreruptFrame* frame);
__attribute__((interrupt)) void KBHandler(struct IntreruptFrame* frame);

void RemapPIC();
void EndMaster();
void EndSlave();