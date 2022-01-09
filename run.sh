#!/bin/bash
RAM="1G"

ARGUMENTS="-drive format=raw,file=kernel/bin/disk.img -bios RELEASEX64_OVMF.fd -serial stdio -m $RAM -machine q35"
ARGUMENTS="$ARGUMENTS -d int -no-reboot -no-shutdown -machine q35,smm=off" # debugging
ARGUMENTS="$ARGUMENTS -s -S" # debugger
qemu-system-x86_64 $ARGUMENTS