KERNEL_SPACE equ 0x10
KERNEL_FS equ 0x30
KERNEL_GS equ 0x38

[extern irq_handler]
[extern isr_handler]

[global debug_ptr]
[global debug_ptr_cr2]
debug_ptr:
    dd 0
debug_ptr_cr2:
    dd 0

[global get_esp]
get_esp:
    mov eax, esp
    ret

[global idt_load]
[extern idt_idtp]
idt_load:
    lidt [idt_idtp]
    ret

__irq_timer:
    pusha

    push ds
    push es
    push fs
    push gs

    mov ax, KERNEL_SPACE
    mov ds, ax
    mov es, ax
    mov ax, KERNEL_FS
    mov fs, ax
    mov ax, KERNEL_GS
    mov gs, ax

    mov ecx, esp
    push ecx

    call irq_handler

    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds

    popa
    add esp, 8

    iret

__irq_common:
    push eax
    mov eax, esp
    push ecx
    push edx
    push ebx
    push eax
    push ebp
    push esi
    push edi

    push ds
    push es
    push fs
    push gs

    mov ax, KERNEL_SPACE
    mov ds, ax
    mov es, ax
    mov ax, KERNEL_FS
    mov fs, ax
    mov ax, KERNEL_GS
    mov gs, ax
    ;mov fs, ax
    ;mov gs, ax

    mov ecx, esp
    push ecx

    call irq_handler

    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds

    ;popa
    pop edi
    pop esi
    pop ebp
    pop ebx
    pop ebx
    pop edx
    pop ecx
    ;pop eax
    add esp, 4 ;discard eax

    add esp, 8

    iret

__isr_common:
    pusha

    push ds
    push es
    push fs
    push gs

    mov ax, KERNEL_SPACE
    mov ds, ax
    mov es, ax
    mov ax, KERNEL_FS
    mov fs, ax
    mov ax, KERNEL_GS
    mov gs, ax

    mov ecx, esp
    push ecx

    call isr_handler

    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds

    popa
    add esp, 8

    iret

%macro __irq_init_save_eax 1

[global __irq%1]

__irq%1:
    cli
    push dword 0
    push dword (%1 + 32)
    jmp __irq_timer
%endmacro

%macro __irq_init 1

[global __irq%1]

__irq%1:
    cli
    push dword 0
    push dword (%1 + 32)
    jmp __irq_common
%endmacro

__irq_init_save_eax 0
__irq_init 1
__irq_init 2
__irq_init 3
__irq_init 4
__irq_init 5
__irq_init 6
__irq_init 7
__irq_init 8
__irq_init 9
__irq_init 10
__irq_init 11
__irq_init 12
__irq_init 13
__irq_init 14
__irq_init 15
__irq_init 121

%macro __isr_init 1

[global __isr%1]

__isr%1:
    cli
    push dword 0
    push dword %1
    jmp __isr_common
%endmacro

%macro __isr_init_dbg 1

[global __isr%1]

__isr%1:
    cli
    pop eax
    mov [debug_ptr],eax
    push dword 0
    push dword %1
    jmp __isr_common
    jmp $
%endmacro

%macro __isr_init_err 1

[global __isr%1]

__isr%1:
    cli
    push eax
    mov eax, cr2
    mov [debug_ptr_cr2], eax
    pop eax
    push dword %1
    jmp __isr_common
%endmacro

__isr_init 0
__isr_init 1
__isr_init 2
__isr_init 3
__isr_init 4
__isr_init 5
__isr_init_dbg 6
__isr_init 7
__isr_init_err 8
__isr_init 9
__isr_init_err 10
__isr_init_err 11
__isr_init_err 12
__isr_init_err 13
__isr_init_err 14
;__isr_init_dbg 14
__isr_init 15
__isr_init 16
__isr_init 17
__isr_init 18
__isr_init 19
__isr_init 20
__isr_init 21
__isr_init 22
__isr_init 23
__isr_init 24
__isr_init 25
__isr_init 26
__isr_init 27
__isr_init 28
__isr_init 29
__isr_init 30
__isr_init 31
