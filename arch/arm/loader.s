interrupt_vector_table:
	b . @Reset
	b . @Undef
	b . @SWI
	b . @Prefetch
	b . @Data abrt
	b . @IRQ
	b . @FIQ

.global _start
_start:
	.globl _start
	ldr sp, =stack_top
	bl _main
1:
	b 1b
