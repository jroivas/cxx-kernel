STACKSIZE             equ 0x4000
KERNEL_VIRTUAL        equ 0xC0000000
[extern __initial_stack]
[global pagingEnable]
pagingEnable:
	push ebp

	mov ax, 0x1741
	mov [0xB8004],ax

	; Enable paging
	mov ebp, esp
	mov eax, [ebp+8]

	;mov edx, cr0
	;and edx, 0x80000000
	mov ax, 0x1742
	mov [0xB8004],ax

	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx
	jmp $

	pop ebp


	ret

	;mov cr0, edx

	mov ax, 0x1743
	mov [0xB8004],ax
	lea ecx, [__reloadStack]
	jmp ecx
	
__reloadStack:
	mov ax, 0x1744
	mov [0xB8004],ax
	mov esp, __initial_stack+STACKSIZE           ; set up the stack
	mov ax, 0x1745
	mov [0xB8004],ax
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
