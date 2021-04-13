#pragma once
#include <stdint.h>

struct RSDP2 {
    unsigned char Signature[8];
    uint8_t Checksum;
    uint8_t ID[6];
    uint8_t Revision;
    uint32_t RSDTAddress;
    uint32_t Lenght;
    uint64_t XSDTAddress;
    uint8_t ExtendedChecksum;
    uint8_t Reserved[3];
} __attribute__((packed));

struct SDT {
    unsigned char Signature[4];
    uint32_t Lenght;
    uint8_t Revision;
    uint8_t Checksum;
    uint8_t ID[6];
    uint8_t TableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} __attribute__((packed));

struct MCFG
{
   SDT Header;
   uint64_t Reserved;
} __attribute__((packed));

struct DeviceConfig
{
    uint64_t BaseAddress;
    uint16_t PCISegmentGroup;
    uint8_t StartBus;
    uint8_t EndBus;
    uint32_t Reserved;
}__attribute__((packed));

struct MADT {
    SDT SDTHeader;
    uint32_t LocalAPICAddress; //moldu' you're an idiot!!! it isn't a pointer...
    uint32_t Flags;
}__attribute__((packed));

struct MADTRecord {
	uint8_t	Type;
	uint8_t	Length;
} __attribute__((packed));

struct LocalProcessor {
	MADTRecord	        Record;
	uint8_t				ProcessorID;
	uint8_t				ApicID;
	uint32_t			Flags;
} __attribute__((packed));

struct IoAPIC {
	MADTRecord	        Record;
	uint8_t				ApicID;
	uint8_t				Reserved;
	uint32_t		    ApicAdress;
	uint32_t		    GlobalIntrerruptBase;
} __attribute__((packed));

struct InterruptSourceOverride {
	MADTRecord	        Record;
	uint8_t				Bus;		
	uint8_t				Source;
	uint32_t			GlobalIntrerrupt;
	uint16_t			Flags;
} __attribute__((packed));

struct NonMaskableInterrupt {
	MADTRecord	        Record;
	uint8_t				ApicID;
	uint16_t			Flags;
	uint8_t				Lint;
} __attribute__((packed));

struct LocalAPICAddressOverride {
	MADTRecord	        Record;
	uint8_t				reserved[2];
	uint64_t			LocalAPICAddress;
} __attribute__((packed));

#define MADT_LOCAL_PROCESSOR 0
#define	MADT_IOAPIC 1
#define	MADT_INTERRUPT_SOURCE_OVERRIDE	2
#define	MADT_NONMASKABLE_INTERRUPT	4
#define	MADT_LOCAL_APICADDRESS_OVERRIDE 5

struct GenericAddressStructure
{
  uint8_t AddressSpace;
  uint8_t BitWidth;
  uint8_t BitOffset;
  uint8_t AccessSize;
  uint64_t Address;
};

struct FADT
{
    SDT header;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;
    uint8_t  Reserved;
    uint8_t  PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t  AcpiEnable;
    uint8_t  AcpiDisable;
    uint8_t  S4BIOS_REQ;
    uint8_t  PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t  PM1EventLength;
    uint8_t  PM1ControlLength;
    uint8_t  PM2ControlLength;
    uint8_t  PMTimerLength;
    uint8_t  GPE0Length;
    uint8_t  GPE1Length;
    uint8_t  GPE1Base;
    uint8_t  CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t  DutyOffset;
    uint8_t  DutyWidth;
    uint8_t  DayAlarm;
    uint8_t  MonthAlarm;
    uint8_t  Century;
    uint16_t BootArchitectureFlags;
    uint8_t  Reserved2;
    GenericAddressStructure ResetReg;
    uint8_t  ResetValue;
    uint8_t  Reserved3[3];
    uint64_t X_FirmwareControl;
    uint64_t X_Dsdt;
    GenericAddressStructure X_PM1aEventBlock;
    GenericAddressStructure X_PM1bEventBlock;
    GenericAddressStructure X_PM1aControlBlock;
    GenericAddressStructure X_PM1bControlBlock;
    GenericAddressStructure X_PM2ControlBlock;
    GenericAddressStructure X_PMTimerBlock;
    GenericAddressStructure X_GPE0Block;
    GenericAddressStructure X_GPE1Block;
}__attribute__((packed));

struct BGRT {
    SDT header;
    uint16_t Version;
    uint8_t Status;
    uint8_t ImageType;
    uint64_t ImageAddress;
    uint32_t OffsetX;
    uint32_t OffsetY;
}__attribute__((packed));

class ACPI
{
public:
    LocalProcessor* CPUCores[1024]; int CPUCoreCount = 0;
    IoAPIC* IOApics[1024]; int IOApicsCount = 0;
    InterruptSourceOverride* IntreruptSourceOverrides[1024]; int IntreruptSourceOverridesCount = 0;
    NonMaskableInterrupt* NMIs[1024]; int NMIsCount = 0;
    LocalAPICAddressOverride* APICAddressOverrides[1024]; int APICAddressOverridesCount = 0;
    FADT* fadt;

    uint16_t SLP_TYPa;
    uint16_t SLP_TYPb;
    uint16_t SLP_EN;
    uint16_t ShutdownPossible;

    void* ShutdownBackup;
    void* RebootBackup;

    void* FindTable(SDT* sdt, char* sign); 
    void ParseMADT(MADT* madt);
    void DoACPIReboot();
    void DoACPIShutdown();
    
};