#include <stdint.h>
#include <stddef.h>
#include <efi.h>
#include <efilib.h>
#include <elf.h>

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
    Print(L"An error occured when loading LLOS!\n\r");
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
    EFI_FILE* llosFolder = OpenFile(NULL,L"LLOS");
    if (llosFolder == NULL) DoError(L"Failed to open the LLOS folder!\n\r");
    EFI_FILE* kernel = OpenFile(llosFolder,L"kernel.llexec");
    if (kernel == NULL) DoError(L"Failed to open the kernel!\n\r");
    EFI_FILE* font = OpenFile(llosFolder,L"font.psf");
    if (font == NULL) DoError(L"Failed to open the font!\n\r");
    EFI_FILE* llfs = OpenFile(llosFolder,L"ram.llfs");
    if (llfs == NULL) DoError(L"Failed to open the LLFS!\n\r");

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

    //Get memory map
	EFI_MEMORY_DESCRIPTOR *Map = NULL;
	UINTN MapSize, MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	EFI_STATUS st = BS->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	BS->AllocatePool(EfiLoaderData, MapSize, (void **)&Map);
	do {
			st = BS->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	} while(st != EFI_SUCCESS);

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

    BS->ExitBootServices(IH, MapKey);
    
    EntryPoint(&binfo);

    while(1) {
        __asm__ volatile ("hlt");
    }
        
	return 1;
}
