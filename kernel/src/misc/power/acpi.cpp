#include "acpi.h"
#include "../../libc/stdio.h"
#include "../../memory/PageTableManager.h"
#include "../../drivers/display/displaydriver.h"
#include "../../io/serial.h"

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

void ACPI::DoACPIReboot() {
    GlobalCOM1->Write("Rebooting");
    outportb(fadt->ResetReg.Address,fadt->ResetValue); //if that fails, we use uefi runtime services
    GlobalCOM1->Write("Try #1 failed!");
    ((void(*)())RebootBackup)();
    GlobalCOM1->Write("Try #2 failed!");
}

void ACPI::DoACPIShutdown() {
    GlobalCOM1->Write("Shutdowning");
    if(ShutdownPossible) {
        outportw(fadt->PM1aControlBlock,SLP_TYPa | SLP_EN);
        outportw(fadt->PM1bControlBlock,SLP_TYPb | SLP_EN);    
    } // if that fails, we try emulator specific metods
    GlobalCOM1->Write("Try #2 failed!");
    
    outportw(0xB004,0x2000);
    outportw(0x604,0x2000);
    outportw(0x4004,0x3400);
    outportw(0x4004,(((5 & 7) << 10) | (1 << 13)));

    GlobalCOM1->Write("Try #2 failed!");

    //if that fails we use uefi runtime services
    ((void(*)())ShutdownBackup)();

    GlobalCOM1->Write("Try #3 failed!");

    //it that fails too we panic 'cause we have nothing left to do to shutdown :(
    IntreruptFrame f = {0};
    KernelPanic("Shutdown failed",&f); 
}

void ACPI::ParseMADT(MADT* madt) {
    uint64_t entries = (madt->SDTHeader.Lenght - sizeof(MADT));
 
    for(uint64_t i = 0;i<entries;) {
        MADTRecord* currentRecord = (MADTRecord*)((uint64_t)madt + sizeof(MADT) + i);
        GlobalTableManager.MapMemory(currentRecord,currentRecord);
        //printf("Record %u: Type: %u Lenght: %u\n",i,currentRecord->Type,currentRecord->Length);
        i += currentRecord->Length; //every type of record has another lenght so we increment by that length
        switch (currentRecord->Type)
        {
        case 0:{
            LocalProcessor* a = (LocalProcessor*)currentRecord;
            CPUCores[CPUCoreCount++] = a;
            printf("Found a CPU core! Processor ID: %u APIC ID: %u Flags: %u\n",a->ProcessorID,a->ApicID,a->Flags);
        }
            break;
        case 1:{
            IoAPIC* a = (IoAPIC*)currentRecord;
            IOApics[IOApicsCount++] = a;
            printf("Found an I/O APIC! Address: %x APIC ID: %u Global Intrerrupt Base: %u\n",a->ApicAdress,a->ApicID,a->GlobalIntrerruptBase);
        }
            break;
        case 2: {
            InterruptSourceOverride* a = (InterruptSourceOverride*)currentRecord;
            IntreruptSourceOverrides[IntreruptSourceOverridesCount++] = a;
            printf("Found an Interrupt Source Override! Bus: %u Flags: %u Global Intrerrupt: %u\n",a->Bus,a->Flags,a->GlobalIntrerrupt);
        }
            break;
        case 4:{
            NonMaskableInterrupt* a = (NonMaskableInterrupt*)currentRecord;
            NMIs[NMIsCount++] = a;
            printf("Found a Non-maskable interrupts! APIC ID: %u Flags: %u Lint: %u\n",a->ApicID,a->Flags,a->Lint);
        }
            break;    
        case 5:{
            LocalAPICAddressOverride* a = (LocalAPICAddressOverride*)currentRecord;
            APICAddressOverrides[APICAddressOverridesCount++] = a;
            printf("Found a Local APIC Address Override! Local APIC Address: %x\n",a->LocalAPICAddress);
        }
            break;
    
        default:
            break;
        }
        GlobalDisplay->update();
    }
}