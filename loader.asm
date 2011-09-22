[BITS 32]

MODULEALIGN equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
FLAGS       equ  MODULEALIGN | MEMINFO  ; this is the Multiboot 'flag' field
MAGIC       equ    0x1BADB002           ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum required

[global start]
[extern _main]
[extern _atexit]
[extern main]

section .text
align 4
MultiBootHeader:
   dd MAGIC
   dd FLAGS
   dd CHECKSUM

STACKSIZE equ 0x4000

start:
	mov esp, stack+STACKSIZE           ; set up the stack
	push eax                           ; pass Multiboot magic number
	push ebx                           ; pass Multiboot info structure

	call _main
	call main
	call _atexit
	cli
	hlt

section .bss
align 4
stack:
   resb STACKSIZE                     ; reserve 16k stack on a doubleword boundary
