#include <stdint.h>
#include <stddef.h>
#include <efi.h>
#include <efilib.h>
#include <elf.h>

/* Data */

//bool variable
#define true 1
#define false 0

#define DECIMAL 10
#define HEX     16

typedef unsigned int            bool; 

struct PageMapIndexer {
    uint64_t PDP_i;
    uint64_t PD_i;
    uint64_t PT_i;
    uint64_t P_i;
};

enum PT_Flag {
    Present = 0,
    ReadWrite = 1,
    WriteThrough = 3,
    CacheDisabled = 4,
    Accessed = 5,
};

struct PageTable { 
    uint64_t entries[512];
}__attribute__((aligned(0x1000)));


/* Code */
void mymemset(void* start, uint8_t value, uint64_t num){
    for (uint64_t i = 0; i < num; i++){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}

uint64_t SetFlag(uint64_t Value, int flag, bool enabled){
    uint64_t bitSelector = (uint64_t)1 << flag;
    Value &= ~bitSelector;
    if (enabled){
        Value |= bitSelector;
    }
    return Value;
}

bool GetFlag(uint64_t Value, int flag){
    uint64_t bitSelector = (uint64_t)1 << flag;
    return (Value & bitSelector) > 0 ? true : false;
}

uint64_t GetAddress(uint64_t Value){
    return (Value & 0x000ffffffffff000) >> 12;
}

uint64_t SetAddress(uint64_t Value, uint64_t address){
    address &= 0x000000ffffffffff;
    Value &= 0xfff0000000000fff;
    Value |= (address << 12);
	return Value;
}


void* allocatePage(){
    EFI_PHYSICAL_ADDRESS PhysicalBuffer;
    ST->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData,1, &PhysicalBuffer);
    return (void*)PhysicalBuffer;
}

void* allocatePages(uint64_t pages){
    EFI_PHYSICAL_ADDRESS PhysicalBuffer;
    ST->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, pages, &PhysicalBuffer);
    return (void*)PhysicalBuffer;
}

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize){
    uint64_t memorySizeBytes = 0;

    for (uint64_t i = 0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
        memorySizeBytes += desc->NumberOfPages * 0x1000;
    }
    return memorySizeBytes;
}

struct PageMapIndexer PageMapIndexer(uint64_t virtualAddress){
    struct PageMapIndexer pageMapIndexer;
    virtualAddress >>= 12;
    pageMapIndexer.P_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    pageMapIndexer.PT_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    pageMapIndexer.PD_i = virtualAddress & 0x1ff;
    virtualAddress >>= 9;
    pageMapIndexer.PDP_i = virtualAddress & 0x1ff;
    return pageMapIndexer;
}

void MapMemory(void* PML4, void* virtualAddress, void* physicalAddress){
    struct PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualAddress);
    uint64_t PDE;
    
    struct PageTable* Table = (struct PageTable*)PML4;
    PDE = Table->entries[indexer.PDP_i];
    uint64_t PDP;
    if (!GetFlag(PDE, 0)){
        PDP = (uint64_t)allocatePage();
        mymemset((void*)PDP, 0, 0x1000);
        PDE = SetAddress(PDE, PDP >> 12);
        PDE = SetFlag(PDE, 0, true);
        PDE = SetFlag(PDE, 1, true);
        Table->entries[indexer.PDP_i] = PDE;
    }
    else
    {
        PDP = (uint64_t)GetAddress(PDE) << 12;
    }
    
    Table = (struct PageTable*)PDP;
    PDE = Table->entries[indexer.PD_i];
    uint64_t PD;
    if (!GetFlag(PDE, 0)){
        PD = (uint64_t)allocatePage();
        mymemset((void*)PD, 0, 0x1000);
        PDE = SetAddress(PDE, (uint64_t)PD >> 12);
        PDE = SetFlag(PDE, 0, true);
        PDE = SetFlag(PDE, 1, true);
        Table->entries[indexer.PD_i] = PDE;
    }
    else
    {
        PD = (uint64_t)GetAddress(PDE) << 12;
    }

    Table = (struct PageTable*)PD;
    PDE = Table->entries[indexer.PT_i];
    uint64_t PT;
    if (!GetFlag(PDE, 0)){
        PT = (uint64_t)allocatePage();
        mymemset((void*)PT, 0, 0x1000);
        PDE = SetAddress(PDE, (uint64_t)PT >> 12);
        PDE = SetFlag(PDE, 0, true);
        PDE = SetFlag(PDE, 1, true);
        Table->entries[indexer.PT_i] = PDE;
    }
    else
    {
        PT = (uint64_t)GetAddress(PDE) << 12;
    }

    Table = (struct PageTable*)PT;
    PDE = Table->entries[indexer.P_i];
    PDE = SetAddress(PDE, (uint64_t)physicalAddress >> 12);
    PDE = SetFlag(PDE, 0, true);
    PDE = SetFlag(PDE, 1, true);
    Table->entries[indexer.P_i] = PDE;
}

void allocatePagesVirtualAddress(void* PML4, uint64_t pages, void* segment){
    for(uint64_t i = 0; i < pages; i += 0x1000){
        void* PhysicalBuffer = allocatePage();
        MapMemory(PML4, (void*)segment + i, (void*)PhysicalBuffer);
    }
}

void mymemcpy(void *d, const void *s, size_t n)
{
    uint64_t d0, d1, d2;
    __asm__ volatile(
        "rep ; movsq\n\t""movq %4,%%rcx\n\t""rep ; movsb\n\t": "=&c" (d0),
        "=&D" (d1),
        "=&S" (d2): "0" (n >> 3),
        "g" (n & 7),
        "1" (d),
        "2" (s): "memory"
    );  //not mine (found it on reddit: https://www.reddit.com/r/C_Programming/comments/ivoqhk/understanding_the_assembly_code_of_memcpy/)
}

void* InitMemory(EFI_MEMORY_DESCRIPTOR* Map, uint64_t MapSize, uint64_t MapDescSize){
    void* PML4 = allocatePage();
    mymemset(PML4, 0, 0x1000);

	//copy uefi's page table
	uint64_t uefiPageTable;
	__asm__  volatile("mov %%cr3, %0" : "=r"(uefiPageTable));
	mymemcpy(PML4,(void*)uefiPageTable,0x1000);
    uint64_t MapEntries = MapSize / MapDescSize;

    uint64_t MemorySize = GetMemorySize(Map, MapEntries, MapDescSize);
    MapMemory(PML4, (void*)Map, (void*)Map);

    __asm__ volatile("mov %0, %%cr3" :: "r" (PML4));
    return PML4;
}

typedef struct {
	void* BaseAddr;
	uint64_t BufferSize;
	uint64_t Width;
	uint64_t Height;
	uint64_t PixelPerScanLine;
} DisplayBuffer;

typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSFHeader;

typedef struct {
    PSFHeader* header;
	void* glyphBuffer;
} PSFFont;

typedef struct {
	DisplayBuffer* framebuffer;
	PSFFont* font;
	EFI_MEMORY_DESCRIPTOR *mMap;
	UINTN mMapSize;
	UINTN mMapDescSize;
	void* RSDP;
	void* SMBIOS;
	void* LLFS;
} BootInfo;

EFI_HANDLE IH;

UINTN strcmp(CHAR8* a,CHAR8* b,UINTN length) {
	for(UINTN i = 0;i < length;i++)
		if(*a != *b) return 0;
	return 1;
}

EFI_FILE* OpenFile(EFI_FILE* Directory, wchar_t* File) {
	EFI_FILE* LoadedFile = NULL;
	EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;

	ST->BootServices->HandleProtocol(IH, &gEfiLoadedImageProtocolGuid, (void **)&LoadedImage);
	ST->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void **)&FileSystem);

	if (Directory == NULL) FileSystem->OpenVolume(FileSystem, &Directory);

	EFI_STATUS status = Directory->Open(Directory, &LoadedFile, File, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	
    if (status != EFI_SUCCESS) LoadedFile = NULL;
	return LoadedFile;
}

void DoError(wchar_t* error) {
    Print(L"An error occured when loading moldOS!\n\r");
    Print(error);
    while(1)
        __asm__ volatile ("hlt");
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    //Initialize some things
    InitializeLib(ImageHandle, SystemTable);
    BS = SystemTable->BootServices;
    ST = SystemTable;
    IH = ImageHandle;

    //Open the required files
    EFI_FILE* moldOSFolder = OpenFile(NULL,L"moldOS");
    if (moldOSFolder == NULL) DoError(L"Failed to open the moldOS folder!\n\r");
    EFI_FILE* kernel = OpenFile(moldOSFolder,L"kernel.melf");
    if (kernel == NULL) DoError(L"Failed to open the kernel!\n\r");
    EFI_FILE* font = OpenFile(moldOSFolder,L"font.psf");
    if (font == NULL) DoError(L"Failed to open the font!\n\r");
    EFI_FILE* llfs = OpenFile(moldOSFolder,L"ram.llfs");
    if (llfs == NULL) DoError(L"Failed to open the LLFS!\n\r");

	/* Load paging */
	EFI_MEMORY_DESCRIPTOR* Map = NULL;
	UINTN MapSize = 0;
	UINTN MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	EFI_STATUS Status = SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);

	Status = EFI_LOAD_ERROR;
	while(Status != EFI_SUCCESS){
		Status = SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	}	

	void* PML4 = InitMemory(Map, MapSize, DescriptorSize);

	uint64_t MapEntries = MapSize / DescriptorSize;
	GetMemorySize(Map, MapEntries, DescriptorSize);
	Print(L"OK");

    //Read the ELF header
    Elf64_Ehdr header;
	UINTN size = sizeof(header);
	kernel->Read(kernel, &size, &header);

    //Verify the header
	if (header.e_ident[EI_CLASS] != ELFCLASS64 || header.e_ident[EI_DATA] != ELFDATA2LSB || header.e_type != ET_EXEC || header.e_machine != EM_X86_64)
		DoError(L"The kernel is corrupt or isn't supported!\n\r");

    //Load the PHeaders
	Elf64_Phdr* phdrs;

	kernel->SetPosition(kernel, header.e_phoff);
	UINTN kernelSize = header.e_phnum * header.e_phentsize;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, kernelSize, (void **)&phdrs);
	kernel->Read(kernel, &kernelSize, phdrs);

	for (Elf64_Phdr *phdr = phdrs; (char *)phdr < (char *)phdrs + header.e_phnum * header.e_phentsize; phdr = (Elf64_Phdr *)((char *)phdr + header.e_phentsize)) {
		switch (phdr->p_type) {
		case PT_LOAD: {
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

				kernel->SetPosition(kernel, phdr->p_offset);
				UINTN phdrSize = phdr->p_filesz;
				kernel->Read(kernel, &phdrSize, (void *)segment);
				break;
			}
		}
	}

    //Load the font
	PSFHeader* fontHeader;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSFHeader), (void **)&fontHeader);
	UINTN psfHeaderSize = sizeof(PSFHeader);
	font->Read(font, &psfHeaderSize, fontHeader);

	if (fontHeader->magic[0] != 0x36 || fontHeader->magic[1] != 0x04)
		DoError(L"Unknown PSF font magic\n\r");

	UINTN glyphBufferSize = fontHeader->charsize * 256;
	void* glyphBuffer;
	font->SetPosition(font, sizeof(PSFHeader));
	SystemTable->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (void **)&glyphBuffer);
	font->Read(font, &glyphBufferSize, glyphBuffer);

	PSFFont* finishedFont;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSFFont), (void **)&finishedFont);
	finishedFont->header = fontHeader;
	finishedFont->glyphBuffer = glyphBuffer;

    //Load LLFS
	UINTN llfsSize = 0xFFFFFF;
	void* llfsBuffer;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, llfsSize, (void **)&llfsBuffer);
	llfs->Read(llfs, &llfsSize, llfsBuffer);

    //Enable GOP
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    DisplayBuffer framebuffer;

	if (BS->LocateProtocol(&gopGuid, ((void *)0), (void **)&gop) != EFI_SUCCESS)
		DoError(L"Cannot init GOP!\n\r");
	
	framebuffer.BaseAddr = (void *)gop->Mode->FrameBufferBase;
	framebuffer.BufferSize = gop->Mode->FrameBufferSize;
	framebuffer.Width = gop->Mode->Info->HorizontalResolution;
	framebuffer.Height = gop->Mode->Info->VerticalResolution;
	framebuffer.PixelPerScanLine = gop->Mode->Info->PixelsPerScanLine;

    //Get RSDP
	EFI_CONFIGURATION_TABLE* configTable = ST->ConfigurationTable;
	void* RSDP = NULL;
	EFI_GUID ACPITABLEGUID = ACPI_20_TABLE_GUID;

	for(UINTN i = 0;i < SystemTable->NumberOfTableEntries;i++) {
		if(CompareGuid(&configTable[i].VendorGuid, &ACPITABLEGUID)) 
			for(UINTN i = 0;i < 8;i++)
				if(strcmp((CHAR8*)"RSD PTR ", (CHAR8*)configTable->VendorTable,8)) 
					RSDP = (void*)configTable->VendorTable;
		configTable++;
	}

	//Get SMBIOS
	configTable = ST->ConfigurationTable;
	void* SMBIOS = NULL;
	EFI_GUID SMBIOSGUID = SMBIOS_TABLE_GUID;
	for(UINTN i = 0;i < SystemTable->NumberOfTableEntries;i++) {
		if(CompareGuid(&configTable[i].VendorGuid, &SMBIOSGUID)){
			if(strcmp((CHAR8*)"_SM_", (CHAR8*)configTable->VendorTable,4)) {
				SMBIOS = (void*)configTable->VendorTable;
				break;
			}
		}
			
		configTable++;
	}

    //Prepare to run the kernel
    int (*EntryPoint)(BootInfo *) = ((__attribute__((sysv_abi)) int (*)(BootInfo *))header.e_entry);
    
    BootInfo binfo;
    binfo.font = finishedFont;
    binfo.framebuffer = &framebuffer;
    binfo.LLFS = llfsBuffer;
    
    binfo.mMap = Map;
    binfo.mMapDescSize = DescriptorSize;
    binfo.mMapSize = MapSize;

    binfo.RSDP = RSDP;
	binfo.SMBIOS = SMBIOS;

    BS->ExitBootServices(IH, MapKey);
    
    EntryPoint(&binfo);

    while(1) {
        __asm__ volatile ("hlt");
    }
        
	return 1;
}
