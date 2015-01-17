ENTRY(_start)
SECTIONS
{
    . = 0x10000;
    .text : {
       code = .;
       *(.text*)
       *(.gnu.linkonce.t*)
    }
    .data : { *(.data) }
    .bss : { *(.bss) }
    .rodata : {
       rodata = .;

       start_ctors = .;
       *(.ctor*)
       end_ctors = .;

       init_array_start = .;
       KEEP (*(.init_array*))
       init_array_end = .;

       start_dtors = .;
       *(.dtor*)
       end_dtors = .;

       *(.gnu.linkonce.r*)

       *(.rodata*)
    }

    . = . + 0x20000; /* Stack */
    stack_top = .;
    end = .; _end = .; __end = .; kernel_end = .;
}
