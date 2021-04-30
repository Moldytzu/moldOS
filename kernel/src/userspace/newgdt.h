#pragma once
#include <stdint.h>

struct table_ptr {
    uint16_t limit;
    uint64_t base;
}__attribute__((packed));

extern "C" void load_gdt(table_ptr * gdt_ptr);
void setup_gdt();
extern "C" void setr0stack(uint64_t rsp);