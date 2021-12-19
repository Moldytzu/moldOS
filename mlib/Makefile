#defs
SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
CROSSCOMPILERBIN = $(HOME)/cross_compiler/bin
PREFIX = $(CROSSCOMPILERBIN)/x86_64-elf-

LDS = linker.ld
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
ASM = nasm
AR = $(PREFIX)ar
CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -static -Iinc/ -mcmodel=large

OUTFILE = $(BUILDDIR)/mlib.a

LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib --no-relax
ASMFLAGS = 

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)     
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)

OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))

DIRS = $(wildcard $(SRCDIR)/*)

#default
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo Compiling: $^
	@ mkdir -p $(@D)
	@ $(CC) $(CFLAGS) -c $^ -o $@
	
$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo Assembling: $^
	@ mkdir -p $(@D)
	@ $(ASM) $(ASMFLAGS) $^ -f elf64 -o $@

#other
link: $(OBJS)
	@ echo Linking: $(OBJS)
	@ strip $(OBJS) --strip-unneeded
	@ $(AR) rcs $(OUTFILE) $(OBJS)
