lock_address    equ 0x2000
base_address    equ 0x8000
STACKSIZE       equ 0x4000

[org base_address]
[bits 16]

smp_init:
    cli
    cld
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax
    mov esp, __initial_stack + STACKSIZE

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
.desc
    dw $ - tempgdt - 1
    dd tempgdt

smp_prepare_32:
    lgdt [tempgdt.desc]

    mov eax, cr0
    or al, 0x01
    mov cr0, eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    jmp tempgdt.code:entry_32

[bits 32]
entry_32:
    mov ax, tempgdt.data
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov esp, __initial_stack + STACKSIZE

    mov word [lock_address], 1

    mov edx, 0xBA420042
    sti

    jmp 0x100000

    hlt

[section .bss]
ALIGN 4
__initial_stack:
    resb STACKSIZE ; reserve 16k stack on a doubleword boundary
