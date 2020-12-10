#include "efiMemory.h"

const char* EFI_MEMORY_TYPE_STRINGS[] {

    "EfiReservedMemoryType",
    "EfiLoaderCode",
    "EfiLoaderData",
    "EfiBootServicesCode",
    "EfiBootServicesData",
    "EfiRuntimeServicesCode",
    "EfiRuntimeServicesData",
    "EfiConventionalMemory",
    "EfiUnusableMemory",
    "EfiACPIReclaimMemory",
    "EfiACPIMemoryNVS",
    "EfiMemoryMappedIO",
    "EfiMemoryMappedIOPortSpace",
    "EfiPalCode",
};

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* nMap, uint64_t nMapEntries, uint64_t nMapDescSize) {
    static uint64_t memorySizeBytes = 0;
    if(memorySizeBytes > 0) return memorySizeBytes;
    for(int i = 0;i < nMapEntries;i++) {
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)nMap + (i * nMapDescSize));
		memorySizeBytes = memorySizeBytes + (desc->numPages * 4096);
	}

    return memorySizeBytes;
}