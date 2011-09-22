[BITS 32]

[section .text]
MODULEALIGN equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
FLAGS       equ  MODULEALIGN | MEMINFO  ; this is the Multiboot 'flag' field
MAGIC       equ  0x1BADB002           ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum required

STACKSIZE equ 0x4000

[global start]
[extern _main]
[extern _atexit]
[extern main]

ALIGN 4
MultiBootHeader:
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

start:
;	lgdt [trickgdt]
;	mov ax, 0x10
;	mov ds, ax
;	mov es, ax
;	mov fs, ax
;	mov gs, ax
;	mov ss, ax

;	jmp 0x08:higherhalf
;
;higherhalf:
	mov esp, stack+STACKSIZE           ; set up the stack
	push eax                           ; pass Multiboot magic number
	push ebx                           ; pass Multiboot info structure

	call _main
	call main
	call _atexit
	cli
	hlt

	;jmp $

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

;[section .setup]
;trickgdt:
;	dw gdt_end - gdt - 1
;	dd gdt
; 
;gdt:
;	dd 0, 0							; null gate
;	db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x40	; code selector 0x08: base 0x40000000, limit 0xFFFFFFFF, type 0x9A, granularity 0xCF
;	db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x40	; data selector 0x10: base 0x40000000, limit 0xFFFFFFFF, type 0x92, granularity 0xCF
; 
;gdt_end:

[section .bss]
align 4
   resb STACKSIZE                     ; reserve 16k stack on a doubleword boundary
stack:
   resb STACKSIZE                     ; reserve 16k stack on a doubleword boundary

