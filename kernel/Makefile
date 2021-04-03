SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi

GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = kern.ld
CC = gcc
LD = ld
ASM = nasm

CFLAGS = -fno-exceptions -ffreestanding -fshort-wchar -mno-red-zone -msse3 -Wno-pointer-arith -Wno-int-to-pointer-cast
# uncomment for debugging
#CFLAGS += -g

LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib
ASMFLAGS = 

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)     
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)

OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))

DIRS = $(wildcard $(SRCDIR)/*)

kernel: $(OBJS) link

$(OBJDIR)/intrerupts/intrerupts.o: $(SRCDIR)/intrerupts/intrerupts.cpp
	@ echo Compiling: $^
	@ mkdir -p $(@D)
	@ $(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -g -c $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo Compiling: $^
	@ mkdir -p $(@D)
	@ $(CC) $(CFLAGS) -c $^ -o $@
	
$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo Assembling: $^
	@ mkdir -p $(@D)
	@ $(ASM) $(ASMFLAGS) $^ -f elf64 -o $@

link: $(OBJS)
	@ echo Linking: $(OBJS)
	@ $(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.llexec $(OBJS)

buildimg: $(BUILDDIR)/kernel.llexec
	@ echo Creating disk image
	@ dd if=/dev/zero of=$(BUILDDIR)/disk.img bs=1024 count=32768
	@ mkfs.vfat $(BUILDDIR)/disk.img
	@ mmd -i $(BUILDDIR)/disk.img ::/EFI
	@ mmd -i $(BUILDDIR)/disk.img ::/LLOS
	@ mmd -i $(BUILDDIR)/disk.img ::/EFI/BOOT
	@ mcopy -i $(BUILDDIR)/disk.img $(GNUEFI)/x86_64/bootloader/BOOTx64.efi ::/EFI/BOOT
	@ mcopy -i $(BUILDDIR)/disk.img font.psf ::/LLOS
	@ mcopy -i $(BUILDDIR)/disk.img $(BUILDDIR)/kernel.llexec ::/LLOS

	@ echo Creating iso image
	@ mkdir -p $(BUILDDIR)/iso
	@ mkdir -p $(BUILDDIR)/iso/LLOS
	@ mkdir -p $(BUILDDIR)/iso/EFI
	@ mkdir -p $(BUILDDIR)/iso/EFI/BOOT
	@ cp $(GNUEFI)/x86_64/bootloader/BOOTx64.efi $(BUILDDIR)/iso/EFI/BOOT
	@ cp font.psf $(BUILDDIR)/iso/LLOS/
	@ cp $(BUILDDIR)/kernel.llexec $(BUILDDIR)/iso/LLOS/
	@ cd $(BUILDDIR)
	@ xorrisofs -r -J -o $(BUILDDIR)/cdrom.iso $(BUILDDIR)/iso
	@ tput sgr0
	
	@ rm -rf $(BUILDDIR)/iso