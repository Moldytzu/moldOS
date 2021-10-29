#include <misc/power/acpi.h>
#include <libc/stdio.h>
#include <memory/PageTableManager.h>
#include <drivers/display/display.h>
#include <io/serial.h>
#include <settings.h>

void* ACPIFindTable(SDT* sdt, char* sign) {
    int entries = (sdt->Lenght - sizeof(SDT)) / 8;
    for(int t = 0;t<entries;t++) {
        SDT* h = (SDT*)*(uint64_t*)((uint64_t)sdt + sizeof(SDT) + (t*8));

        #ifdef Debugging_ACPI
            printf("Found table: %c%c%c%c\n",h->Signature[0],h->Signature[1],h->Signature[2],h->Signature[3]);GlobalDisplay->update();
        #endif

        for(int i = 0;i<4;i++) {
            if(h->Signature[i] != sign[i])
                break;
            if(i == 3) return h;
        }
    }
    return 0;
}