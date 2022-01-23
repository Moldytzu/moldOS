OUTPUT_FORMAT(elf64-x86-64)
ENTRY(moldMain)

SECTIONS
{
	. = 0xFFFF800000000000;
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