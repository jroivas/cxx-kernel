.section .text
.global _reset
_reset:
interrupt_vector_table:
    /*b . @Reset*/
    b _start
    b . @Undef
    b . @SWI
    b . @Prefetch
    b . @Data abrt
    b . @IRQ
    b . @FIQ

/*.section .init*/
.global _start
.globl _start
_start:
    ldr sp, =stack_top

    ldr r0,=0x20200000
    mov r1,#1
    lsl r1,#18
    str r1,[r0,#4]

    mov r1,#1
    lsl r1,#16

    loop$:

str r1,[r0,#40]

mov r2,#0x3F0000
wait1$:
    sub r2,#1
    cmp r2,#0
    bne wait1$

str r1,[r0,#28]

mov r2,#0x3F0000
wait2$:
    sub r2,#1
    cmp r2,#0
    bne wait2$
/*
b loop$
*/
    /*bl _textual

    ldr r0,=0x20200000
    mov r1,#1
    lsl r1,#16
    */
    str r1,[r0,#40]


    bl _main 

    ldr r0,=0x20200000
    mov r1,#1
    lsl r1,#16
    str r1,[r0,#28]

1:
    b 1b
