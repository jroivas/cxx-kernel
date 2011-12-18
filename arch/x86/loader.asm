[BITS 32]
[global loaderstart]
[extern _main]
[extern _atexit]

MULTIBOOT_ALIGN       equ  1<<0                   ; align loaded modules on page boundaries
MULTIBOOT_MEMINFO     equ  1<<1                   ; provide memory map
MULTIBOOT_FLAGS       equ  MULTIBOOT_ALIGN | MULTIBOOT_MEMINFO  ; this is the Multiboot 'flag' field
MULTIBOOT_MAGIC       equ  0x1BADB002           ; 'magic number' lets bootloader find the header
MULTIBOOT_CHECKSUM    equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)        ; checksum required

STACKSIZE             equ 0x4000


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
	mov [multiboot_magic_data], eax
	mov [multiboot_info_data], ebx

__call_kernel:
	mov esp, __initial_stack+STACKSIZE           ; set up the stack

	; pass Multiboot magic number
	mov eax, [multiboot_magic_data]
	push eax

	; pass Multiboot info structure
	mov ebx, [multiboot_info_data]
	push ebx

	call _main
	call _atexit
	cli
	hlt

[global __initial_stack]
	
[section .bss]
ALIGN 4
;ALIGN 32
__initial_stack:
   resb STACKSIZE                     ; reserve 16k stack on a doubleword boundary
