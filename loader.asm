[BITS 32]
[global start]
[extern _main]
[extern _atexit]
;[extern main]

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

boot_page_dir:
	dd (boot_page_table + KERNEL_PAGE_FLAGS - KERNEL_VIRTUAL)
	times (KERNEL_PAGE - 1) dd 0
kernel_page:
	dd (boot_page_table + KERNEL_PAGE_FLAGS - KERNEL_VIRTUAL)
	times (1024 - KERNEL_PAGE - 2) dd 0
	dd (boot_page_dir + KERNEL_PAGE_FLAGS - KERNEL_VIRTUAL)
boot_page_table:
	times (1024) dd 0
boot_page_end:

multiboot_data:
	multiboot_magic_data dd 0
	multiboot_info_data  dd 0

[section .text]
ALIGN 4

multiboot_header:
	dd MULTIBOOT_MAGIC
	dd MULTIBOOT_FLAGS
	dd MULTIBOOT_CHECKSUM

start:
;	mov ecx, eax
;	lgdt [trickgdt]
;	mov ax, 0x10
;	mov ds, ax
;	mov es, ax
;	mov fs, ax
;	mov gs, ax
;	mov ss, ax
;	jmp 0x08:higherhalf
 
higherhalf:
	mov [multiboot_magic_data - KERNEL_VIRTUAL], eax
	;mov [multiboot_magic_data - KERNEL_VIRTUAL], ecx
	mov [multiboot_info_data - KERNEL_VIRTUAL], ebx
	;mov ax, 0x1741
	;mov [0xB8000],ax

	;xor ebx, ebx
	;add ebx, 3

	mov ebx, KERNEL_PAGE_FLAGS
	mov ecx, (boot_page_table - KERNEL_VIRTUAL)

loop:
	mov [ecx], ebx
	add ecx, 4 		;32 bit
	add ebx, 0x1000
	cmp ecx, (boot_page_end - KERNEL_VIRTUAL)
	jb loop

	mov ecx, (boot_page_dir - KERNEL_VIRTUAL)
	mov cr3, ecx

	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx

	lea ecx, [high_half]
	jmp ecx
	;jmp 0x08:high_half
	
high_half:
	mov dword [boot_page_dir], 0
	invlpg [0]

	;mov ax, 0x1742
	;mov [KERNEL_VIRTUAL+0xB8000],ax

	mov esp, stack+STACKSIZE           ; set up the stack

	; pass Multiboot magic number
	mov eax, [multiboot_magic_data]
	push eax

	; pass Multiboot info structure
	mov ebx, [multiboot_info_data]
	add ebx, KERNEL_VIRTUAL
	push ebx

	;push eax                           ; pass Multiboot magic number
	;push ebx                           ; pass Multiboot info structure

	call _main
;	call main
	call _atexit
	cli
	hlt


[global gdt_flush]
[extern gdt_ptr] 
gdt_flush:
	lgdt [gdt_ptr]
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:gdt_flush_exit

gdt_flush_exit:
	ret

[section .bss]
ALIGN 4
stack:
   resb STACKSIZE                     ; reserve 16k stack on a doubleword boundary

[section .setup] ; tells the assembler to include this data in the '.setup' section
 
trickgdt:
	dw gdt_end - gdt - 1 ; size of the GDT
	dd gdt ; linear address of GDT
 
gdt:
	dd 0, 0							; null gate
	db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x40	; code selector 0x08: base 0x40000000, limit 0xFFFFFFFF, type 0x9A, granularity 0xCF
	db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x40	; data selector 0x10: base 0x40000000, limit 0xFFFFFFFF, type 0x92, granularity 0xCF
 
gdt_end:
