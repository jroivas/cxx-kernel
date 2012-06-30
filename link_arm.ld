ENTRY(loaderstart)
SECTIONS
{
 . = 0x10000;
 .text : { *(.text) }
 .data : { *(.data) }
 .bss : { *(.bss) }
/*
 . = . + 0x1000;
 stack_top = .;
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

	*(.rodata*)
	*(.gnu.linkonce.r*)
	. = ALIGN(4096);
 }
}
