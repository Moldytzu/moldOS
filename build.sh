#clean up
./clean.sh

#bootloader
cd gnu-efi
make -j bootloader
cd ..

#apps
for d in ./apps/* ; do (cd "$d" && echo "Compilling $d" && make && cp ./bin/* ../../ramfs/); done

#ramfs
python3 buildramfs.py

#kernel
cd kernel
make -s -j kernel
