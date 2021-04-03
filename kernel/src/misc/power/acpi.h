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
    uint32_t* LocalAPICAddress;
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

class ACPI
{
public:
    void* FindTable(SDT* sdt, char* sign); 
};