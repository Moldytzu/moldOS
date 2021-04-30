#include "newgdt.h"




struct gdt_entry {
  uint16_t limit15_0;            uint16_t base15_0;
  uint8_t  base23_16;            uint8_t  type;
  uint8_t  limit19_16_and_flags; uint8_t  base31_24;
}__attribute__((packed));


struct tss {
    uint32_t reserved0; uint64_t rsp0;      uint64_t rsp1;
    uint64_t rsp2;      uint64_t reserved1; uint64_t ist1;
    uint64_t ist2;      uint64_t ist3;      uint64_t ist4;
    uint64_t ist5;      uint64_t ist6;      uint64_t ist7;
    uint64_t reserved2; uint16_t reserved3; uint16_t iopb_offset;
} __attribute__((packed)) tss_t;


struct {
  struct gdt_entry null;
  struct gdt_entry kernel_code;
  struct gdt_entry kernel_data;
  struct gdt_entry null2;
  struct gdt_entry user_data;
  struct gdt_entry user_code;
  struct gdt_entry ovmf_data;
  struct gdt_entry ovmf_code;
  struct gdt_entry tss_low;
  struct gdt_entry tss_high;
} __attribute__((aligned(4096))) gdt_table = {
    {0, 0, 0, 0x00, 0x00, 0},  /* 0x00 null  */
    {0, 0, 0, 0x9a, 0xa0, 0},  /* 0x08 kernel code (kernel base selector) */
    {0, 0, 0, 0x92, 0xa0, 0},  /* 0x10 kernel data */
    {0, 0, 0, 0x00, 0x00, 0},  /* 0x18 null (user base selector) */
    {0, 0, 0, 0x92, 0xa0, 0},  /* 0x20 user data */
    {0, 0, 0, 0x9a, 0xa0, 0},  /* 0x28 user code */
    {0, 0, 0, 0x92, 0xa0, 0},  /* 0x30 ovmf data */
    {0, 0, 0, 0x9a, 0xa0, 0},  /* 0x38 ovmf code */
    {0, 0, 0, 0x89, 0xa0, 0},  /* 0x40 tss low */
    {0, 0, 0, 0x00, 0x00, 0},  /* 0x48 tss high */
};


uint64_t stack[2048];


void memzero(void * s, uint64_t n) {
    for (int i = 0; i < n; i++) ((uint8_t*)s)[i] = 0;
}

void setup_gdt() {
    memzero((void*)&tss_t, sizeof(tss));
    uint64_t tss_base = ((uint64_t)&tss_t);
    gdt_table.tss_low.base15_0 = tss_base & 0xffff;
    gdt_table.tss_low.base23_16 = (tss_base >> 16) & 0xff;
    gdt_table.tss_low.base31_24 = (tss_base >> 24) & 0xff;
    gdt_table.tss_low.limit15_0 = sizeof(tss);
    gdt_table.tss_high.limit15_0 = (tss_base >> 32) & 0xffff;
    gdt_table.tss_high.base15_0 = (tss_base >> 48) & 0xffff;

    table_ptr gdt_ptr = { sizeof(gdt_table)-1, (uint64_t)&gdt_table };
    load_gdt(&gdt_ptr);
}

void setr0stack(uint64_t rsp) {
  tss_t.rsp0 = rsp;
}