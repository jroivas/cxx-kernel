/*OUTPUT_FORMAT("binary")*/
ENTRY(start)
SECTIONS
{
/*	. = 0x100000; */
	. = 0x00100000;

	.text ALIGN(4096) :
	{
		*(.text*)
		*(.gnu.linkonce.t*)
	}

	.rodata ALIGN(4096) :
	{
		start_ctors = .;
		*(.ctor*)
		end_ctors = .;

		start_dtors = .;
		*(.dtor*)
		end_dtors = .;

		*(.rodata*)
		*(.gnu.linkonce.r*)
	}
	
	.data ALIGN(4096) :
	{
		*(.data*)
		*(.gnu.linkonce.d*)
	}

	.bss ALIGN(4096) :
	{
		sbss = .;
		*(.COMMON*)
		*(.bss*)
		*(.gnu.linkonce.b*)
		ebss = .;
	}
	
	 /DISCARD/ :
	 {
		*(.comment)
		*(.eh_frame) /* You should discard this unless you're implementing runtime support for C++ exceptions. */
	 }
}
