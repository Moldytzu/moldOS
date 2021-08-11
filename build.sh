#clean up
rm -rf kernel/lib
mkdir kernel/lib
rm -rf kernel/bin
mkdir kernel/bin

#bootloader
cd gnu-efi
make -s -j bootloader
cd ..

#apps
for d in ./apps/* ; do (cd "$d" && echo "Compilling $d" && make && cp ./bin/* ../../ramfs/); done

#ramfs
python3 buildramfs.py

#kernel
cd kernel
make -s -j kernel