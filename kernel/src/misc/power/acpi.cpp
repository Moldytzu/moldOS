#include "acpi.h"
#include "../../libc/stdio.h"
#include "../../memory/PageTableManager.h"
#include "../../drivers/display/displaydriver.h"

void* ACPI::FindTable(SDT* sdt, char* sign) {
    int entries = (sdt->Lenght - sizeof(SDT)) / 8;
    for(int t = 0;t<entries;t++) {
        SDT* h = (SDT*)*(uint64_t*)((uint64_t)sdt + sizeof(SDT) + (t*8));
        //printf("%c%c%c%c\n",h->Signature[0],h->Signature[1],h->Signature[2],h->Signature[3]);GlobalDisplay->update();
        for(int i = 0;i<4;i++) {
            if(h->Signature[i] != sign[i])
                break;
            if(i == 3) return h;
        }
    }
    return 0;
}



void ACPI::ParseMADT(MADT* madt) {
    uint64_t entries = (madt->SDTHeader.Lenght - sizeof(MADT));
 
    for(uint64_t i = 0;i<entries;) {
        MADTRecord* currentRecord = (MADTRecord*)((uint64_t)madt + sizeof(MADT) + i);
        GlobalTableManager.MapMemory(currentRecord,currentRecord);
        //printf("Record %u: Type: %u Lenght: %u\n",i,currentRecord->Type,currentRecord->Length);GlobalDisplay->update();
        i += currentRecord->Length; //every type of record has another lenght so we increment by that length
        switch (currentRecord->Type)
        {
        case 0:
            printf("Found a CPU core!\n");GlobalDisplay->update();
            break;
        case 1:
            printf("Found an I/O APIC!\n");GlobalDisplay->update();
            break;
        case 2:
            printf("Found an Interrupt Source Override!\n");GlobalDisplay->update();
            break;
        case 4:
            printf("Found a Non-maskable interrupts!\n");GlobalDisplay->update();
            break;    
        case 5:
            printf("Found a Local APIC Address Override!\n");GlobalDisplay->update();
            break;
    
        default:
            break;
        }
    }
}