STACKSIZE             equ 0x4000
;KERNEL_VIRTUAL        equ 0xC0000000
KERNEL_VIRTUAL        equ 0x0

stackpos:
	dd 0
val1:
	dd 0
target:
	dd 0
retaddr:
	dd 0

[extern __initial_stack]
[global pagingEnable]
pagingEnable:
	mov [stackpos], esp

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	;lea ecx, [__reloadStack]
	;jmp ecx

	ret


[global pagingDisable]
pagingDisable:
	; Disable paging
	mov edx, cr0
	and edx, 0x7fffffff
	mov cr0, edx

	ret

[global pagingDirectoryChange]
pagingDirectoryChange:
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]
	mov cr3, eax
	pop ebp

	ret

[GLOBAL copyPhysicalPage]
copyPhysicalPage:
	push ebx
	pushf

	cli

	mov ebx, [esp+12]
	mov ecx, [esp+16]

	;call pagingDisable
	; Disable paging
	mov edx, cr0
	and edx, 0x7fffffff
	mov cr0, edx

	mov edx, 1024

.copyloop:
	mov eax, [ebx]
	mov [ecx], eax
	add ebx, 4
	add ecx, 4
	dec edx
	jnz .copyloop

	;call pagingEnable
	; Enable paging
	mov edx, cr0
	and edx, 0x80000000
	mov cr0, edx

	popf
	pop ebx
	ret
