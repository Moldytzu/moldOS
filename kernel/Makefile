include ../std.mk

GNUEFI = ../gnu-efi

# sources
SRC = $(call rwildcard,$(SRCDIR),*.cpp)     
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)

# objects
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))

kernel: buildimg

#default
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo Compiling: $^
	@ mkdir -p $(@D)
	@ $(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo Assembling: $^
	@ mkdir -p $(@D)
	@ $(ASM) $^ -f elf64 -o $@

#other
link: $(OBJS)
	@ echo Linking: $(OBJS)
	@ $(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.melf $(OBJS)

stripbinaries: link
	#@ strip $(GNUEFI)/x86_64/bootloader/BOOTx64.efi --strip-unneeded
	#@ strip $(BUILDDIR)/kernel.melf --strip-unneeded

buildimg: stripbinaries
	@ echo Creating disk image
	@ dd if=/dev/zero of=$(BUILDDIR)/disk.img bs=1024 count=32768
	@ mkfs.vfat $(BUILDDIR)/disk.img
	@ mmd -i $(BUILDDIR)/disk.img ::/EFI
	@ mmd -i $(BUILDDIR)/disk.img ::/moldOS
	@ mmd -i $(BUILDDIR)/disk.img ::/EFI/BOOT
	@ mcopy -i $(BUILDDIR)/disk.img $(GNUEFI)/x86_64/bootloader/BOOTx64.efi ::/EFI/BOOT
	@ mcopy -i $(BUILDDIR)/disk.img font.psf ::/moldOS
	@ mcopy -i $(BUILDDIR)/disk.img $(BUILDDIR)/ram.llfs ::/moldOS
	@ mcopy -i $(BUILDDIR)/disk.img $(BUILDDIR)/kernel.melf ::/moldOS

	@ echo Creating iso image
	@ mkdir -p $(BUILDDIR)/iso
	@ mkdir -p $(BUILDDIR)/iso/moldOS
	@ mkdir -p $(BUILDDIR)/iso/EFI
	@ mkdir -p $(BUILDDIR)/iso/EFI/BOOT
	@ cp $(GNUEFI)/x86_64/bootloader/BOOTx64.efi $(BUILDDIR)/iso/EFI/BOOT
	@ cp font.psf $(BUILDDIR)/iso/moldOS/
	@ cp $(BUILDDIR)/kernel.melf $(BUILDDIR)/iso/moldOS/
	@ cp $(BUILDDIR)/ram.llfs $(BUILDDIR)/iso/moldOS/
	@ cd $(BUILDDIR)
	@ xorrisofs -r -J -o $(BUILDDIR)/cdrom.iso $(BUILDDIR)/iso
	
	@ rm -rf $(BUILDDIR)/iso