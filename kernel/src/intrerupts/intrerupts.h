#pragma once
#include "../drivers/display/displaydriver.h"
#include "../io/serial.h"

struct IntreruptFrame;
__attribute__((interrupt)) void DivByZeroHandler(struct interrupt_frame* frame); //0
__attribute__((interrupt)) void DebugHandler(struct interrupt_frame* frame); //1
__attribute__((interrupt)) void BoundRangeHandler(struct interrupt_frame* frame); //5
__attribute__((interrupt)) void InvalidOpcodeHandler(struct interrupt_frame* frame); //6
__attribute__((interrupt)) void DeviceUnavailableHandler(struct interrupt_frame* frame); //7
__attribute__((interrupt)) void InvalidTSSHandler(struct interrupt_frame* frame); //10
__attribute__((interrupt)) void SegmentNotPresentHandler(struct interrupt_frame* frame); //11
__attribute__((interrupt)) void StackFaultHandler(struct interrupt_frame* frame); //12
__attribute__((interrupt)) void GeneralProtectionFaultHandler(struct interrupt_frame* frame);//13
__attribute__((interrupt)) void PageFaultHandler(struct interrupt_frame* frame); //14
__attribute__((interrupt)) void FloatingPointHandler(struct interrupt_frame* frame); //16
__attribute__((interrupt)) void AligmentHandler(struct interrupt_frame* frame); //17
__attribute__((interrupt)) void SMIDFloatHandler(struct interrupt_frame* frame); //19
__attribute__((interrupt)) void VirtualizationFaultHandler(struct interrupt_frame* frame); //20
__attribute__((interrupt)) void SecurityExceptionHandler(struct interrupt_frame* frame); //30