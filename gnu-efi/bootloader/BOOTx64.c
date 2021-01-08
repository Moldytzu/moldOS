#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include <stddef.h>

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct {
	void* BaseAddr;
	long long BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelPerScanLine;
} framebuffer;

typedef struct  {
	CHAR16* Vendor;
	uint32_t Version;
} UEFIFirmware;

typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

typedef struct {
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
} PSF1_FONT;

typedef struct {
	framebuffer* framebuf;
	PSF1_FONT* font;
	void* PowerDownVoid;
	UEFIFirmware* firm;
	EFI_MEMORY_DESCRIPTOR* mMap;
	UINTN mMapSize;
	UINTN mMapDescSize;
} BootInfo;


const wchar_t* LLOSLogo = L"/ \\   / \\   / \\   /  _ \\/ ___\\\n\r" //
			   "| |   | |   | |   | / \\||    \\\n\r" //
			   "| |_/\\| |_/\\| |_/\\| \\_/|\\___ |\n\r" //
			   "\\____/\\____/\\____/\\____/\\____/\n\r";


void PowerDown() {
	ST->RuntimeServices->ResetSystem(EfiResetShutdown,0,0,NULL);
}

void ClearScreen() {
	uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut); 
}

void InitUEFI(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	ST = SystemTable; //Ceva initializare
	InitializeLib(ImageHandle, SystemTable);
	SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);//dezactivare blocare dupa 5 minuturi
}

EFI_FILE* ReadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable){
	EFI_FILE* LoadedFile;

	EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
	SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
	SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);

	if (Directory == NULL){
		FileSystem->OpenVolume(FileSystem, &Directory);
	}

	EFI_STATUS s = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (s != EFI_SUCCESS){
		return NULL;
	}
	return LoadedFile;
}

framebuffer frambuf;

framebuffer* InitGOP() {
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	EFI_STATUS stat;

	stat = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);;
	if(EFI_ERROR(stat)) {
		ClearScreen();
		Print(L"Cannot init GOP!\n\r");
		while(1) {}
	}
	else {
		frambuf.BaseAddr = (void*)gop->Mode->FrameBufferBase;
		frambuf.BufferSize = gop->Mode->FrameBufferSize;
		frambuf.Width = gop->Mode->Info->HorizontalResolution;
		frambuf.Height = gop->Mode->Info->VerticalResolution;
		frambuf.PixelPerScanLine = gop->Mode->Info->PixelsPerScanLine;
		return &frambuf;
	}
}

PSF1_FONT* LoadFont(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
	EFI_FILE* font = ReadFile(Directory, Path, ImageHandle, SystemTable);
	if (font == NULL) return NULL;

	PSF1_HEADER* fontHeader;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void**)&fontHeader);
	UINTN size = sizeof(PSF1_HEADER);
	font->Read(font, &size, fontHeader);

	if (fontHeader->magic[0] != PSF1_MAGIC0 || fontHeader->magic[1] != PSF1_MAGIC1){
		return NULL;
	}

	UINTN glyphBufferSize = fontHeader->charsize * 256;
	if (fontHeader->mode == 1) { //512 glyph mode
		glyphBufferSize = fontHeader->charsize * 512;
	}

	void* glyphBuffer;
	{
		font->SetPosition(font, sizeof(PSF1_HEADER));
		SystemTable->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (void**)&glyphBuffer);
		font->Read(font, &glyphBufferSize, glyphBuffer);
	}

	PSF1_FONT* finishedFont;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_FONT), (void**)&finishedFont);
	finishedFont->psf1_Header = fontHeader;
	finishedFont->glyphBuffer = glyphBuffer;
	return finishedFont;
}

int memcmp(const void* aptr,const void* bptr,size_t n) {
	const unsigned char* a = aptr,*b = bptr;
	for(size_t i = 0;i<n;i++) {
		if(a[i]<b[i]) return -1;
		else if(a[i] > b[i]) return 1;
	}
	return 0;
}


void TriggerError(wchar_t* errstr) {
	ClearScreen();
	Print(errstr);
	while(1){}
}

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	InitUEFI(ImageHandle, SystemTable);

	ClearScreen();
	uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, EFI_YELLOW);
	Print(LLOSLogo);
	Print(L"=====[Loading Kernel....]=====");

	EFI_FILE* monkernel = ReadFile(NULL,L"kernel.llexec",ImageHandle,SystemTable); if(monkernel == NULL) TriggerError(L"Cannot find \"kernel.llexec\"!");
	
	Elf64_Ehdr header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO* FileInfo;
		monkernel->GetInfo(monkernel,&gEfiFileInfoGuid,&FileInfoSize,NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
		monkernel->GetInfo(monkernel,&gEfiFileInfoGuid,&FileInfoSize,(void**)&FileInfo);
		
		UINTN size = sizeof(header);
		monkernel->Read(monkernel,&size,&header);
	}
		
	if(memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 || header.e_ident[EI_CLASS] != ELFCLASS64 || header.e_ident[EI_DATA] != ELFDATA2LSB || header.e_type != ET_EXEC || header.e_machine != EM_X86_64 || header.e_version != EV_CURRENT)  TriggerError(L"Cannot verify the kernel!");
	
	Elf64_Phdr* phdrs;
	{
		monkernel->SetPosition(monkernel, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
		monkernel->Read(monkernel, &size, phdrs);
	}

	for (Elf64_Phdr* phdr = phdrs;(char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize;phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize))
	{
		switch (phdr->p_type){
			case PT_LOAD:
			{
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

				monkernel->SetPosition(monkernel, phdr->p_offset);
				UINTN size = phdr->p_filesz;
				monkernel->Read(monkernel, &size, (void*)segment);
				break;
			}
		}
	}
			
			
	PSF1_FONT* newFont = LoadFont(NULL, L"font.psf",ImageHandle,SystemTable); if(newFont == NULL) TriggerError(L"Cannot load \"font.psf\"!");

	framebuffer* buf = InitGOP();
			
	EFI_MEMORY_DESCRIPTOR* Map = NULL;
	UINTN MapSize, MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	{
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
		SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	}

	int	(*EntryPoint)(BootInfo*) = ((__attribute__((sysv_abi)) int (*)(BootInfo*)) header.e_entry);

	UEFIFirmware f;
	f.Vendor = SystemTable->FirmwareVendor;
	f.Version = SystemTable->FirmwareRevision;

	BootInfo info;
	info.framebuf = buf;
	info.font = newFont;
	info.PowerDownVoid = PowerDown;
	info.mMap = Map;
	info.mMapSize = MapSize;
	info.mMapDescSize = DescriptorSize;
	info.firm =  &f;

	SystemTable->BootServices->ExitBootServices(ImageHandle,MapKey);

	EntryPoint(&info); TriggerError(L"Cannot run the kernel!");

	return 1;
}
