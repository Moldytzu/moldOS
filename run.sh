#!/bin/bash

ARGUMENTS="-drive format=raw,file=kernel/bin/disk.img -bios RELEASEX64_OVMF.fd -serial stdio -m 128M -machine q35"
ARGUMENTS="$ARGUMENTS -d int -no-reboot -no-shutdown -machine q35,smm=off"
qemu-system-x86_64 $ARGUMENTS