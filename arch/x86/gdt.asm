[global gdt_flush]
[extern __gdt_ptr]
gdt_flush:
    lgdt [__gdt_ptr]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    ;mov fs, ax
    ;mov gs, ax
    mov ax, 0x30
    ;or ax, 3
    mov fs, ax
    ;gs
    mov ax, 0x38
    ;or ax, 3
    mov gs, ax
    jmp 0x08:__gdt_flush_exit

__gdt_flush_exit:
    ret


[global tss_flush]
tss_flush:
    mov ax, 0x2B ; TSS is sixth entry in GDT
    ;mov ax, 0x28 ; TSS is sixth entry in GDT
    ltr ax
    ret
