
OSNAME = LLOS

GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = kern.ld
CC = gcc
LD = ld

CFLAGS = -ffreestanding -fshort-wchar
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)          
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
DIRS = $(wildcard $(SRCDIR)/*)

kernel: $(OBJS) link

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@
	
link:
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS)
	
setup:
	@mkdir $(BUILDDIR)
	@mkdir $(SRCDIR)
	@mkdir $(OBJDIR)

buildimg:
	mv $(BUILDDIR)/kernel.elf $(BUILDDIR)/kernel.llexec
	mv $(GNUEFI)/x86_64/bootloader/main.efi $(GNUEFI)/x86_64/bootloader/BOOTx64.efi
	
	#dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=2880
	#mformat -i $(BUILDDIR)/$(OSNAME).img -f 2880 ::
	#mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	#mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	#mcopy -i $(BUILDDIR)/$(OSNAME).img $(GNUEFI)/x86_64/bootloader/BOOTx64.efi ::/EFI/BOOT
	#mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	#mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernel.llexec ::

	dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=1024 count=32768
	mkfs.vfat $(BUILDDIR)/$(OSNAME).img
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(GNUEFI)/x86_64/bootloader/BOOTx64.efi ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img a.txt ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/zap-light16.psf ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernel.llexec ::
	
	

run:
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none
