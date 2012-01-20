[global gdt_flush]
[extern __gdt_ptr]
gdt_flush:
        lgdt [__gdt_ptr]
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        jmp 0x08:__gdt_flush_exit

__gdt_flush_exit:
        ret


[global tss_flush]
tss_flush:
	mov ax, 0x2B ; TSS is sixth entry in GDT
	ltr ax
	ret
