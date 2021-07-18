;lock_address    equ 0x2000
;done_address    equ 0x2020
base_address    equ 0x8000

[org base_address]
[bits 16]

smp_init:
    cli
    cld

    in al, 0x92
    or al, 0x02
    out 0x92, al
    jmp smp_prepare_32

tempgdt:
    dq 0x0
.code equ $ - tempgdt
    dq 0xcf9a000000ffff
.data equ $ - tempgdt
    dq 0xcf92000000ffff
.tss equ $ - tempgdt
    dq 0xcf890000000068
.desc:
    dw $ - tempgdt - 1
    dd tempgdt
    dd 0

smp_prepare_32:
    xor ax, ax
    mov ds, ax
    lgdt [tempgdt.desc]

    mov eax, cr0
    or al, 0x01
    mov cr0, eax

    jmp tempgdt.code:entry_32

[bits 32]
entry_32:
    mov ax, tempgdt.data
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

;    mov word [lock_address], 1

;.1:  pause
;    cmp byte [done_address], 0
;    jz .1

    ; 0xBA420042 is magic to detect SMP CPUs
    mov edx, 0xBA420042

    ; jump to loader, FIXME
    ;jmp 0x100000
    jmp 0x100024

    hlt
