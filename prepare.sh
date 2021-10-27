echo "Preparing workspace for moldOS development"
mkdir ramfs
sudo bash -c "apt-get install build-essential mtools xorriso nasm python3 qemu-system-x86 code -y >> /dev/null"
echo "Done!"
