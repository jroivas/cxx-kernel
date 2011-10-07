KERNEL_SPACE equ 0x10

extern irq_handler
[global idt_load]
idt_load:
	push ebp
	mov ebp,esp
	sub esp,0x40
	mov ebx,[ebp+8]
	lidt [ebx]

	leave
	ret

irq_common:
	pusha

	push ds
	push es
	push fs
	push gs

	mov ax, KERNEL_SPACE
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov eax, esp
	push eax

	mov eax, irq_handler
	call eax

	pop eax

	pop gs
	pop fs
	pop es
	pop ds

	popa
	add esp, 8
	iret

%macro irq_init 1

[global irq%1]

irq%1:
	cli
	push byte 0
	push byte (%1 + 32)
	jmp irq_common
%endmacro

irq_init 0
irq_init 1
irq_init 2
irq_init 3
irq_init 4
irq_init 5
irq_init 6
irq_init 7
irq_init 8
irq_init 9
irq_init 10
irq_init 11
irq_init 12
irq_init 13
irq_init 14
irq_init 15
