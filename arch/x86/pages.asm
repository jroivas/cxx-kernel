[global pagingEnable]
pagingEnable:
	; Enable paging
	mov edx, cr0
	and edx, 0x80000000
	mov cr0, edx

	ret

[global pagingDisable]
pagingDisable:
	; Disable paging
	mov edx, cr0
	and edx, 0x7fffffff
	mov cr0, edx

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
