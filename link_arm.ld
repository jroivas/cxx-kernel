ENTRY(_start)
SECTIONS
{
 . = 0x10000;
 /*.startup . : { arch/loader.o(.text) }*/
 .text : {
	*(.text*)
/*
	*(.data)
	*(.bss)
*/


 }
 .data : { *(.data) }
 .bss : { *(.bss) }
 .rodata : {
	rodata = .;

	start_ctors = .;
	*(.ctor*)
	*(.init_array*) /* For ARM */
	end_ctors = .;

	start_dtors = .;
	*(.dtor*)
	end_dtors = .;

	*(.rodata*)
 }

 . = . + 0x20000; /* Stack */
/* . = . + 0x2000; /* Stack */
 stack_top = .;
 end = .; _end = .; __end = .; kernel_end = .;
/*
 .data : { *(.data) }
 .bss : { *(.bss) }
*/
/*
 . = . + 0x1000;
 stack_top = .;
*/
/*
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
*/
}
