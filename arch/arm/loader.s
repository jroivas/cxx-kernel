interrupt_vector_table:
	b . @Reset
	b . @Undef
	b . @SWI
	b . @Prefetch
	b . @Data abrt
	b . @IRQ
	b . @FIQ

.comm stack 0x10000
loaderstart:
_start:
	.globl _start
	ldr sp, =stack+0x10000
	bl main
1:
	b 1b
