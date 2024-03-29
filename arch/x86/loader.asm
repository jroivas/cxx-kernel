[BITS 32]
[global loaderstart]
[extern _main]
[extern _smp_main]
[extern _atexit]
[extern __stack_top]

; align loaded modules on page boundaries
MULTIBOOT_ALIGN       equ  1<<0
; provide memory map
MULTIBOOT_MEMINFO     equ  1<<1
; this is the Multiboot 'flag' field
MULTIBOOT_FLAGS       equ  MULTIBOOT_ALIGN | MULTIBOOT_MEMINFO
; 'magic number' helps bootloader find this header
MULTIBOOT_MAGIC       equ  0x1BADB002
; checksum required
MULTIBOOT_CHECKSUM    equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

lock_address    equ 0x2000
done_address    equ 0x2020

[section .data]
ALIGN 4096

multiboot_data:
    multiboot_magic_data dd 0
    multiboot_info_data  dd 0

[section .text]
ALIGN 4

[extern code]
[extern bss]
[extern kernel_end]
[global my_kernel_end]
multiboot_header:
    dd 0
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM
    dd multiboot_header
    dd code
    dd bss
my_kernel_end:
    dd kernel_end
    dd loaderstart

loaderstart:
    ; 0xBA420042 is magic to detect SMP CPUs, see smp_init.asm
    cmp edx, 0xBA420042
    je loader_smp_init
    mov [multiboot_magic_data], eax
    mov [multiboot_info_data], ebx

__call_kernel:
    ; set up the stack
    mov esp, [__stack_top]

    ; pass Multiboot magic number
    mov eax, [multiboot_magic_data]
    push eax

    ; pass Multiboot info structure
    mov ebx, [multiboot_info_data]
    push ebx

    ; Nullify ebp for stack trace
    xor ebp, ebp
    call _main
    call _atexit
    cli
    hlt

loader_smp_init:
    ; get current core number
    mov eax, 1
    cpuid
    shr ebx, 24

    ; setup stack space according the core
    mov edi, ebx
    shl ebx, 14
    mov esp, [__stack_top]
    sub esp, ebx
    push edi

    ; unlock initialization of next core
    mov word [lock_address], 1
    ; wait until all cores are initialized
.1: pause
    cmp byte [done_address], 0
    jz .1

    ; jump to core specific main handler
    xor ebp, ebp
    call _smp_main
    cli
    hlt


[section .bss]
ALIGN 4
     ; TODO Figure out why we need this?
     resb 0x1000
