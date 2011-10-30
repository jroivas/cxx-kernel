[BITS 32]
[global loaderstart]
[extern _main]
[extern _atexit]

MULTIBOOT_ALIGN       equ  1<<0                   ; align loaded modules on page boundaries
MULTIBOOT_MEMINFO     equ  1<<1                   ; provide memory map
MULTIBOOT_FLAGS       equ  MULTIBOOT_ALIGN | MULTIBOOT_MEMINFO  ; this is the Multiboot 'flag' field
MULTIBOOT_MAGIC       equ  0x1BADB002           ; 'magic number' lets bootloader find the header
MULTIBOOT_CHECKSUM    equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)        ; checksum required

KERNEL_VIRTUAL        equ 0xC0000000
KERNEL_PAGE           equ (KERNEL_VIRTUAL>>22)
KERNEL_PAGE_FLAGS     equ 0x3

STACKSIZE             equ 0x4000


[section .data]
ALIGN 4096

__boot_page_dir:
	dd (__boot_page_table + KERNEL_PAGE_FLAGS - KERNEL_VIRTUAL)
	times (KERNEL_PAGE - 1) dd 0
__kernel_page:
	dd (__boot_page_table + KERNEL_PAGE_FLAGS - KERNEL_VIRTUAL)
	times (1024 - KERNEL_PAGE - 2) dd 0
	dd (__boot_page_dir + KERNEL_PAGE_FLAGS - KERNEL_VIRTUAL)
__boot_page_table:
	times (1024) dd 0
__boot_page_end:

multiboot_data:
	multiboot_magic_data dd 0
	multiboot_info_data  dd 0

[section .text]
ALIGN 4

multiboot_header:
	dd MULTIBOOT_MAGIC
	dd MULTIBOOT_FLAGS
	dd MULTIBOOT_CHECKSUM

loaderstart:
	mov [multiboot_magic_data - KERNEL_VIRTUAL], eax
	mov [multiboot_info_data - KERNEL_VIRTUAL], ebx

	mov ebx, KERNEL_PAGE_FLAGS
	mov ecx, (__boot_page_table - KERNEL_VIRTUAL)

__loop_page:
	mov [ecx], ebx
	add ecx, 4 		;32 bit
	add ebx, 0x1000
	cmp ecx, (__boot_page_end - KERNEL_VIRTUAL)
	jb __loop_page

	mov ecx, (__boot_page_dir - KERNEL_VIRTUAL)
	mov cr3, ecx

	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx

	lea ecx, [__high_half]
	jmp ecx
	
__high_half:
	mov dword [__boot_page_dir], 0
	invlpg [0]

	;mov ax, 0x1742
	;mov [KERNEL_VIRTUAL+0xB8000],ax

	mov esp, __initial_stack+STACKSIZE           ; set up the stack

	; pass Multiboot magic number
	mov eax, [multiboot_magic_data]
	push eax

	; pass Multiboot info structure
	mov ebx, [multiboot_info_data]
	add ebx, KERNEL_VIRTUAL
	push ebx

	call _main
	call _atexit
	cli
	hlt


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

[section .bss]
ALIGN 4
__initial_stack:
   resb STACKSIZE                     ; reserve 16k stack on a doubleword boundary
