echo "Preparing workspace for LLOS development"
mkdir ramfs
sudo bash -c "apt-get install build-essential mtools xorriso nasm python3 qemu-system-x86 code wget -y >> /dev/null"
echo "Done!"
