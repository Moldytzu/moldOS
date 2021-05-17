cd gnu-efi
make -s -j8 bootloader
cd ..
rm -rf kernel/lib
mkdir kernel/lib
cd kernel
rm -rf bin/cdrom.iso
rm -rf bin/disk.img
make -j8 -s kernel