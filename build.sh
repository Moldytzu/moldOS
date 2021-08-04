#clean up
echo "Cleaning up"
rm -rf kernel/lib
mkdir kernel/lib
rm -rf kernel/bin
mkdir kernel/bin
rm -rf gnu-efi/x86_64/bootloader

#bootloader
echo "Building bootloader"
cd gnu-efi
make -s -j bootloader
cd ..

#apps
echo "Building apps"
for d in ./apps/* ; do (cd "$d" && echo "Compilling $d" && make && cp ./bin/* ../../ramfs/); done

#ramfs
echo "Building ramfs"
python3 buildramfs.py

#kernel
echo "Building kernel and image"
cd kernel
make -s -j kernel
