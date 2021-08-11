echo "Preparing workspace for LLOS development (you may need to enter your password)"
mkdir ramfs
sudo bash -c "apt-get install build-essential mtools xorriso nasm python3 qemu-system-x86 code -y >> /dev/null"
echo "Done!"