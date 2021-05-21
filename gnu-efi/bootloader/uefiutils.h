#pragma once
#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include <stddef.h>
#include "clib.h"

void PowerDown() {
	ST->RuntimeServices->ResetSystem(EfiResetShutdown, 0, 0, NULL);
}

void PowerRestart() {
	ST->RuntimeServices->ResetSystem(EfiResetWarm, 0, 0, NULL);
}

void InitUEFI(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	ST = SystemTable; //Ceva initializare
	InitializeLib(ImageHandle, SystemTable);
	SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL); //dezactivare blocare dupa 5 minuturi
}

EFI_FILE *ReadFile(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	EFI_FILE *LoadedFile;

	EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
	SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void **)&LoadedImage);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
	SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void **)&FileSystem);

	if (Directory == NULL)
		FileSystem->OpenVolume(FileSystem, &Directory);

	EFI_STATUS s = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (s != EFI_SUCCESS)
		return NULL;
	return LoadedFile;
}

void* ReadBytes(EFI_FILE* directory,wchar_t* filename, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	EFI_FILE *f = ReadFile(directory, filename, ImageHandle, SystemTable);
	if (f == NULL)
		return 0;
	UINTN size = 0xFFFFFF;
	void *buf;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void **)&buf);
	f->Read(f, &size, buf);
	return buf;
}
