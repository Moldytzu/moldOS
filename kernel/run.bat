@echo off
set OSNAME=LLOS
set BUILDDIR=%0/../bin
set OVMFDIR=%0/../../OVMFbin
set RAM=4G
"%0\..\..\qemu\qemu-system-x86_64.exe" -soundhw pcspk -drive file=%BUILDDIR%/%OSNAME%.img -m %RAM% -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="%OVMFDIR%/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="%OVMFDIR%/OVMF_VARS-pure-efi.fd" -net none >NUL