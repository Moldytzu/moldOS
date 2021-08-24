#clean up
./clean.sh

#bootloader
cd gnu-efi
make -j bootloader
cd ..

#apps
for d in ./apps/* ; do (cd "$d" && echo "Compilling $d" && make && cp ./bin/* ../../ramfs/); done

#ramfs
python3 tools/buildllfs.py ./ramfs/ ./kernel/bin/ram.llfs
python3 tools/buildllfs2.py ./ramfs/ ./kernel/bin/ram.llfs2 

#kernel
cd kernel
make -s -j kernel
