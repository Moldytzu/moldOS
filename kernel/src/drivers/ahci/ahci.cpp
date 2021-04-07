#include "ahci.h"
#include "../../libc/stdio.h"
#include "../../memory/PageTableManager.h"
#include "../../memory/heap.h"

PortType CheckPortType(HBAPort* port) {
    #define atapi 0xEB140101
    #define ata 0x101
    #define semb 0xC33C0101
    #define pm 0x96690101
    
    uint32_t sataStatus = port->sataStatus;
    uint8_t interfacePowerManagement = (sataStatus >> 8) & 0b111;
    uint8_t deviceDetection = sataStatus & 0b111;
    if(deviceDetection != 0x3) return PortType::None;
    if(interfacePowerManagement != 0x1) return PortType::None;
    
    switch (port->signature)
    {
        case atapi:
            return PortType::SATAPI;
        case ata:
            return PortType::SATA;
        case pm:
            return PortType::PM;
        case semb:
            return PortType::SEMB;
        default:
            return PortType::None;
    }
}

void AHCIDriver::ProbePorts() {
    uint32_t portsImplemented = ABAR->portsImplemented;
    for(int i = 0;i<32;i++)
        if(portsImplemented & (1 << i)) {
            PortType type = CheckPortType(&ABAR->ports[i]);
            if(type == PortType::SATA || type == PortType::SATAPI) {
                Ports[PortCount] = new Port();
                Ports[PortCount]->portType = type;
                Ports[PortCount]->hbaport = &ABAR->ports[i];
                Ports[PortCount]->portNumber = PortCount;
                PortCount++;
            }
        }
}

bool Port::Read(uint64_t sector,uint32_t sectorCount,void* buffer) {
        uint32_t sectorL = (uint32_t) sector;
        uint32_t sectorH = (uint32_t) (sector >> 32);

        hbaport->interruptStatus = (uint32_t)-1; // Clear pending interrupt bits

        HBACommandHeader* cmdHeader = (HBACommandHeader*)hbaport->commandListBase;
        cmdHeader->commandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
        cmdHeader->write = 0; //this is a read
        cmdHeader->prdtLength = 1;

        HBACommandTable* commandTable = (HBACommandTable*)(cmdHeader->commandTableBaseAddress);
        memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength-1)*sizeof(HBAPRDTEntry));

        commandTable->prdtEntry[0].dataBaseAdress = (uint32_t)(uint64_t)buffer;
        commandTable->prdtEntry[0].dataBaseAdressUpper = (uint32_t)((uint64_t)buffer >> 32);
        commandTable->prdtEntry[0].byteCount = (sectorCount<<9)-1; // 512 bytes per sector
        commandTable->prdtEntry[0].intrerruptOnCompletion = 1;

        FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->commandFIS);

        cmdFIS->fisType = FIS_TYPE_REG_H2D;
        cmdFIS->commandControl = 1; // command
        cmdFIS->command = ATA_CMD_READ_DMA_EX;

        cmdFIS->lba0 = (uint8_t)sectorL;
        cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
        cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
        cmdFIS->lba3 = (uint8_t)sectorH;
        cmdFIS->lba4 = (uint8_t)(sectorH >> 8);
        cmdFIS->lba5 = (uint8_t)(sectorH >> 16);

        cmdFIS->deviceRegister = 1<<6; //LBA mode

        cmdFIS->countLow = sectorCount & 0xFF;
        cmdFIS->coutHigh = (sectorCount >> 8) & 0xFF;

        uint64_t spin = 0;

        while ((hbaport->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000){
            spin ++;
        }
        if (spin == 1000000) {
            return false;
        }

        hbaport->commandIssue = 1;

        while (true){

            if((hbaport->commandIssue == 0)) break;
            if(hbaport->interruptStatus & HBA_PxIS_TFES)
            {
                return false;
            }
        }

        return true;
}

void Port::Configure() {
    Stop();
    void* base = GlobalAllocator.RequestPage();
    hbaport->commandListBase = (uint32_t)(uint64_t)base;
    hbaport->commandListBaseUpper = (uint32_t)((uint64_t)base >> 32);
    memset((void*)(hbaport->commandListBase),0,1024);
    void* fisBase = GlobalAllocator.RequestPage();
    hbaport->fisBaseAddress = (uint32_t)(uint64_t)fisBase;
    hbaport->fisBaseAddressUpper = (uint32_t)((uint64_t)fisBase >> 32);
    memset(fisBase,0,256);
    HBACommandHeader* cmdHeader = (HBACommandHeader*)((uint64_t)hbaport->commandListBase + ((uint64_t)hbaport->commandListBaseUpper<<32));
    for(int i = 0;i < 32;i++) {
        cmdHeader[i].prdtLength = 8;
        void* cmdTableAddress =GlobalAllocator.RequestPage();
        uint64_t address = (uint64_t)cmdTableAddress + (i << 8);
        cmdHeader[i].commandTableBaseAddress = (uint32_t)(uint64_t)cmdTableAddress;
        cmdHeader[i].commandTableBaseAddressUpper = (uint32_t)((uint64_t)cmdTableAddress >> 32);
        memset(cmdTableAddress,0,256);
    }
    Start();
}

#define HBA_PxCMD_CR 0x8000
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FR 0x4000

void Port::Stop() {
    hbaport->cmdSts &= ~HBA_PxCMD_ST;
    hbaport->cmdSts &= ~HBA_PxCMD_FRE;

    while (1)
    {
        if(hbaport->cmdSts & HBA_PxCMD_FR) continue;
        if(hbaport->cmdSts & HBA_PxCMD_CR) continue;
        break;
    }
    
}

void Port::Start() {
    while(hbaport->cmdSts & 0x8000);

    hbaport->cmdSts |= HBA_PxCMD_FRE;
    hbaport->cmdSts |= HBA_PxCMD_ST;
}

AHCIDriver::AHCIDriver(PCIDevice* pciBaseAddress) {
    this->PCIBaseAdress = pciBaseAddress;
    ABAR = (HBAMemory*)((PCIHeader0*)pciBaseAddress)->BAR5;
    GlobalTableManager.MapMemory(ABAR,ABAR);
    ProbePorts();

    printf("Configuring ports...\n");
    for(int i=0;i < PortCount;i++) {
        Port* port = Ports[i];
        port->Configure();       
    }
    printf("Detected drives:\n");
    for(int i=0;i<PortCount;i++) {
        Port* port = Ports[i];
            
        if(port->portType == PortType::SATA) printf("SATA Drive\n");
        if(port->portType == PortType::SATAPI) printf("SATAPI Drive\n");

        port->buffer = (uint8_t*)GlobalAllocator.RequestPage();
        memset(port->buffer,0,4096);
        port->Read(0,6,port->buffer);
        
        for(int i = 0;i<3072;i++) {
            GlobalDisplay->putc(port->buffer[i]);
        }
        
        GlobalDisplay->cursorNewLine();
        GlobalDisplay->update();
    }
}

AHCIDriver::~AHCIDriver() {
    
}