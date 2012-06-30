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
}
