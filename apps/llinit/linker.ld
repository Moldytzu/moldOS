OUTPUT_FORMAT(binary)

SECTIONS
{
	.text : ALIGN(0x1000)
	{
		*(.text)
		*(.data)
		*(.rodata)
		*(COMMON)
		*(.bss)
	}
}
