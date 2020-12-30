@echo off
set OSNAME=LLOS
set BUILDDIR=%0/../bin
set OVMFDIR=%0/../../OVMFbin
set RAM=8G
set CPU=-cpu core2duo
set QEMU="%0\..\..\qemu\qemu-system-x86_64.exe"
set OVMFARGS=-drive if=pflash,format=raw,unit=0,file="%OVMFDIR%/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="%OVMFDIR%/OVMF_VARS-pure-efi.fd"
%QEMU% -soundhw pcspk -drive file=%BUILDDIR%/%OSNAME%.img -m %RAM% %CPU% %OVMFARGS% -net none >NUL