echo "Preparing workspace for moldOS development"
mkdir ramfs
mkdir mlib/bin
sudo bash -c "apt-get install build-essential mtools xorriso nasm python3 qemu-system-x86 astyle -y >> /dev/null"
echo "Done!"
