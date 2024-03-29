# folders
SRCDIR := src
OBJDIR := lib
BUILDDIR = bin

# cross compiler binaries
CROSSCOMPILERBIN = $(HOME)/cross_compiler/bin
PREFIX = $(CROSSCOMPILERBIN)/x86_64-elf-

# cross compiler defines
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
ASM = nasm
LDS = linker.ld

# flags
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib --no-relax -n
CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -static -Isrc/ -mcmodel=large 
CFLAGS += -finline-functions -fthread-jumps -faggressive-loop-optimizations -fdce -fdse -foptimize-sibling-calls -foptimize-strlen -frounding-math -ffast-math # optimizations
ASMFLAGS = 

# wildcard
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))