#clean up
./clean.sh

#bootloader
cd gnu-efi
make -j bootloader
cd ..

#lib
cd mlib
make
cd ..

#apps
for d in ./apps/* ; do (
    (if [[ $d != "./apps/apps.mk" ]]; then 
        cd "$d" && 
        echo "Compilling $d" && 
        make && 
        cp ./bin/* ../../ramfs/
    fi)
); done

#ramfs
python3 tools/buildllfs.py ./ramfs/ ./kernel/bin/ram.llfs

#kernel
cd kernel
make -s -j$(nproc) kernel
