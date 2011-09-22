/*OUTPUT_FORMAT("binary")*/
/*OUTPUT_FORMAT("elf32-i386")*/
ENTRY(start)
phys = 0x100000;
SECTIONS
{
	. = 0x100000;

/*
	.setup :
	{
		*(.setup)
	}
*/


	/*.text AT(ADDR(.text) - 0xC0000000) ALIGN(4096) :
	.text ALIGN(4096) : AT(ADDR(.text) - 0xC0000000)
	.text : AT(ADDR(.text) - 0xC0000000)
*/
	.text phys : AT(phys)
	{
		code = .;
		*(.text*)
		*(.gnu.linkonce.t*)
		. = ALIGN(4096);
	}

/*
	. += 0xC0000000;
	.rodata ALIGN(4096) : AT(ADDR(.rodata) - 0xC0000000)
	.rodata ALIGN(4096) :
*/
	.rodata : AT(phys + (rodata - code))
	{
		rodata = .;
		start_ctors = .;
		*(.ctor*)
		end_ctors = .;

		start_dtors = .;
		*(.dtor*)
		end_dtors = .;

		*(.rodata*)
		*(.gnu.linkonce.r*)
		. = ALIGN(4096);
	}
	
/*
	.data ALIGN(4096) : AT(ADDR(.data) - 0xC0000000)
	.data ALIGN(4096) :
*/
	.data : AT(phys + (data - code))
	{
		data = .;
		*(.data*)
		*(.gnu.linkonce.d*)
		. = ALIGN(4096);
	}

/*
	.bss ALIGN(4096) : AT(ADDR(.bss) - 0xC0000000)
	.bss ALIGN(4096) :
*/
	.bss : AT(phys + (bss - code))
	{
		bss = .;
		*(.COMMON*)
		*(.bss*)
		*(.gnu.linkonce.b*)
		. = ALIGN(4096);
	}
	
	 /DISCARD/ :
	 {
		*(.comment)
		*(.eh_frame) /* You should discard this unless you're implementing runtime support for C++ exceptions. */
	 }
}
