OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kernelMain)

SECTIONS
{
	_KernelStart = .;
	.text :
	{
		*(.text)
	}
	.data :
	{
		*(.data)
	}
	.rodata :
	{
		*(.rodata)
	}
	_BssStart = .;
	.bss :
	{
		*(COMMON)
		*(.bss)
	}
	_BssEnd = .;
	_KernelEnd = .;
}