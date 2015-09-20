/*OUTPUT_FORMAT("binary")*/
/*
OUTPUT_FORMAT("elf32-i386")
OUTPUT_FORMAT("elf32-i386")
*/
ENTRY(loaderstart)
/*
	phys = 0x100000;
phys = 0xC0000000;
*/
phys = 0x0;
SECTIONS
{
/*
	. = 0x100000;
	. = 0xC0100000;
	.text ALIGN(16) :
	{
		*(.mb*)
	}
	. = 0x100000;
*/


/*
	.setup :
	{
		*(.setup)
	}
*/

/*
	. = 0xC0100000;
*/

	/*.text AT(ADDR(.text) - 0xC0000000) ALIGN(4096) :
	.text ALIGN(4096) : AT(ADDR(.text) - 0xC0000000)
	.text : AT(ADDR(.text) - 0xC0000000)
	.text phys : AT(phys)
*/
/*
	.text : AT(ADDR(.text) - phys)
	. += 0xC0000000;
	. += 0xC0000000;
	.text : AT(ADDR(.text) - phys)
*/
	.text 0x100000 :
	{
		code = .;
		*(.text*)
		*(.gnu.linkonce.t*)

		. = ALIGN(4096);
/*
		. = ALIGN(4096);
*/
	}

/*
	. += 0xC0000000;
	.rodata ALIGN(4096) : AT(ADDR(.rodata) - 0xC0000000)
	.rodata ALIGN(4096) :
	.rodata : AT(phys + (rodata - code))
		. = ALIGN(4096);
	.rodata : AT(ADDR(.rodata) - phys)
*/
	.rodata :
	{
		rodata = .;

		start_ctors = .;
		*(.ctor*)
		end_ctors = .;

		start_dtors = .;
		*(.dtor*)
		end_dtors = .;

		init_array_start = .;
		KEEP (*(.init_array*))
		init_array_end = .;

		*(.rodata*)
/*
		*(.shstrtab*)
		*(.symtab*)
		*(.strtab*)
*/
		*(.gnu.linkonce.r*)
		. = ALIGN(4096);
	}
	
/*
	.data ALIGN(4096) : AT(ADDR(.data) - 0xC0000000)
	.data ALIGN(4096) :
	.data : AT(phys + (data - code))
	.data : AT(phys + (data - code))
		. = ALIGN(4096);
	.data ALIGN(4096) : AT(ADDR(.data) - phys)
*/
	.data :
	{
		data = .;
		*(.data*)
		*(.gnu.linkonce.d*)
		. = ALIGN(4096);
	}

/*
	.bss ALIGN(4096) : AT(ADDR(.bss) - 0xC0000000)
	.bss ALIGN(4096) :
	.bss : AT(phys + (bss - code))
		. = ALIGN(4096);
	.bss : AT(ADDR(.bss) - phys)
*/
	.bss :
	{
		bss = .;
		*(.COMMON*)
		*(.bss*)
		*(.gnu.linkonce.b*)
		. = ALIGN(4096);
	}
	.tdata :
	{
		tdata_data = .;
		*(.tdata .tdata.* .gnu.linkonce.td.*)
		tdata_data_end = .;
	}
	.tbss :
	{
		tbss_data = .;
		*(.tbss .tbss.* .gnu.linkonce.tb.*)
		*(.tcommon)
		tbss_data_end = .;
		. = ALIGN(64);
	}
	.tls_template_size = SIZEOF(.tdata) + SIZEOF(.tbss);
	tcb0 = .;
	. = . + .tls_template_size + 256;
	
	/DISCARD/ :
	{
		*(.comment)
		*(.eh_frame) /* You should discard this unless you're implementing runtime support for C++ exceptions. */
		*(.note*)
		/**(.*debug*)*/
	}

	end = .; _end = .; __end = .; kernel_end = .;

}
/*
PHDRS {
	text PT_LOAD FILEHDR PHDRS;
	tls PT_TLS;
	dynamic PT_DYNAMIC;
	eh_frame PT_GNU_EH_FRAME;
	note PT_NOTE;
}
*/
