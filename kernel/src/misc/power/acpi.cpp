#include "acpi.h"
#include "../../libc/stdio.h"
#include "../../memory/PageTableManager.h"
#include "../../drivers/display/displaydriver.h"

void* ACPI::FindTable(SDT* sdt, char* sign) {
    int entries = (sdt->Lenght - sizeof(SDT)) / 8;
    for(int t = 0;t<entries;t++) {
        SDT* h = (SDT*)*(uint64_t*)((uint64_t)sdt + sizeof(SDT) + (t*8));
        for(int i = 0;i<4;i++) {
            if(h->Signature[i] != sign[i])
                break;
            if(i == 3) return h;
        }
    }
    return 0;
}