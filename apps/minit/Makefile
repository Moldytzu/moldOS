include ../apps.mk

OUTFILE = $(BUILDDIR)/minit.melf

SRC = $(call rwildcard,$(SRCDIR),*.cpp)     
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)

OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))

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
	@ $(LD) $(LDFLAGS) -o $(OUTFILE) $(OBJS) ../../mlib/bin/mlib.a
