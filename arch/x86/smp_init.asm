lock_address: equ 0x2000
base_address: equ 0x8000

org base_address
bits 16

smp_init:
    xor ax, ax
    mov ds, ax

    mov word [lock_address], 1

    ; TODO 32bit and start
    hlt
