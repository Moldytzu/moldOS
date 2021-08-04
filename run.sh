rm -rf /tmp/RELEASEX64_OVMF.fd
wget https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd -P /tmp/
qemu-system-x86_64 -drive format=raw,file=kernel/bin/disk.img -bios /tmp/RELEASEX64_OVMF.fd -serial stdio -m 2G -machine q35
