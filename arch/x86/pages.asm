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
	;push ebp
	;pop eax
	mov [retaddr], eax
	mov [stackpos], esp

	;mov [val1], esp

	mov ax, 0x1741
	mov [0xB8004],ax

	; Enable paging
	;mov ebp, esp
	;mov eax, [ebp+8]

	;mov edx, cr0
	;and edx, 0x80000000
	mov ax, 0x1742
	mov [0xB8004],ax

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

	;pop ebp

	mov ax, 0x1743
	mov [0xB8004],ax

	;jmp $
	
	mov ecx, 0xB8010
	mov [target], ecx
	;mov eax, __reloadStack
	mov eax, [retaddr]

a:
	xor edx, edx
	mov ebx, 10
	div ebx

	mov ecx, [target]
	;mov 
	add dx, 0x1730
	mov [ecx], dx
	inc ecx
	inc ecx
	mov [target], ecx

	cmp eax, 0
	jnz a

	lea ecx, [__reloadStack]

	;pop eax
	mov eax, ecx
	;mov eax, cs
	mov ecx, 0xB80B0
	mov [target], ecx

b:
	xor edx, edx
	mov ebx, 10
	div ebx

	mov ecx, [target]
	;mov 
	add dx, 0x1730
	mov [ecx], dx
	inc ecx
	inc ecx
	mov [target], ecx

	cmp eax, 0
	jnz b

	lea ecx, [__reloadStack]
	jmp ecx

	;jmp 0x08:__reloadStack

	ret

	;mov cr0, edx

	mov ax, 0x1743
	mov [0xB8004],ax
	lea ecx, [__reloadStack]
	jmp ecx
	
__reloadStack:
	mov ax, 0x1744
	mov [0xB8004],ax
	;jmp $

	;mov ax, 0x10
	;mov ss, ax
	;xor eax, eax
	;mov ds, ax
	;mov es, ax
	;mov fs, ax
	;mov gs, ax

	;mov ax, 0x10
	;mov ds, ax
	;mov es, ax
	;mov fs, ax
	;mov gs, ax
	;mov ss, ax
	mov esp, [stackpos]

	mov ax, 0x1745
	mov [0xB8004],ax
	;jmp $

	;mov eax, [retaddr]
	;jmp [eax]
	;jmp $
	ret

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
