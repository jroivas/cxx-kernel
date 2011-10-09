KERNEL_SPACE equ 0x10

[extern irq_handler]
[extern isr_handler]

[global get_esp]
get_esp:
        mov eax, esp
        ret

[global idt_load]
[extern idt_idtp]
idt_load:
	lidt [idt_idtp]
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

isr_common:
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

	mov eax, isr_handler
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

%macro isr_init 1

[global isr%1]

isr%1:
	cli
	push byte 0
	push byte %1
	jmp isr_common
%endmacro

%macro isr_init_err 1

[global isr%1]

isr%1:
	cli
	push byte %1
	jmp isr_common
%endmacro

isr_init 0
isr_init 1
isr_init 2
isr_init 3
isr_init 4
isr_init 5
isr_init 6
isr_init 7
isr_init_err 8
isr_init 9
isr_init_err 10
isr_init_err 11
isr_init_err 12
isr_init_err 13
isr_init_err 14
isr_init 15
isr_init 16
isr_init 17
isr_init 18
isr_init 19
isr_init 20
isr_init 21
isr_init 22
isr_init 23
isr_init 24
isr_init 25
isr_init 26
isr_init 27
isr_init 28
isr_init 29
isr_init 30
isr_init 31
