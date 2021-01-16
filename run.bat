@echo off

set OSNAME=LLOS
set BUILDDIR=kernel/bin
set OVMFDIR=OVMFbin
set RAM=-m 8G
set CPU=-cpu core2duo
set QEMU="qemu\qemu-system-x86_64.exe"
set OVMFARGS=-drive if=pflash,format=raw,unit=0,file="%OVMFDIR%/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="%OVMFDIR%/OVMF_VARS-pure-efi.fd"
set BOOTDEVICE=-drive file=%BUILDDIR%/%OSNAME%.img,format=raw
set QEMUADITIONALARGS=-soundhw pcspk
set QEMUARGS= %BOOTDEVICE% %RAM% %CPU% %OVMFARGS% %QEMUADITIONALARGS%

echo [1m[97m========Rulez hemulatorul========[0m[95m
%QEMU% %QEMUARGS%>NUL
echo [0m