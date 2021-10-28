OUTPUT_FORMAT(elf64-x86-64)
ENTRY(moldMain)

SECTIONS
{
	. = 0x0000100000200004;
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
}