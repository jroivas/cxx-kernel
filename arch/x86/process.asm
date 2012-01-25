TASK_MAGIC equ 0x42

[global saveProcess]
saveProcess:
	mov eax, [esp+4]

	mov ecx, esp
	add ecx, 4

	mov edx, [esp]

	mov [eax], edi
	mov [eax+4], esi
	mov [eax+8], ebx
	mov [eax+12], ebp
	mov [eax+16], ecx
	mov [eax+20], edx

	xor eax, eax
	ret

[global restoreProcess]
restoreProcess:
	mov ecx, [esp+4] ;lock
	mov eax, [esp+8] ;stack

	mov edi, [eax]
	mov esi, [eax+4]
	mov ebx, [eax+8]
	mov ebp, [eax+12]
	mov esp, [eax+16]
	mov edx, [eax+20]
	
	cmp ecx, 0
	jz .restoreProcess_no_lock
	mov dword [ecx], 0
.restoreProcess_no_lock:

	xor eax,eax
	inc eax

	push edx
	ret

[global changeProcess]
changeProcess:
	; FIXME
	mov bx, 0x10
	mov fs, bx
	mov gs, bx

	mov ebp, esp
	mov esi, [ebp+20]  ;killer
	mov edx, [ebp+16]  ;parm
	mov ebx, [ebp+12]  ;stack
	mov eax, [ebp+8]   ;eip
	mov ecx, [ebp+4]   ;lock


	; Change stack
	cmp ebx,0
	jz .changeProcess_no_stack
	mov esp, ebx
.changeProcess_no_stack:

	; Unlocking
	cmp ecx, 0
	jz .changeProcess_no_lock
	mov dword [ecx], 0
.changeProcess_no_lock:

	push edx
	push esi ; Killer, when this thread exit

	xor ebp, ebp
	sti
	jmp eax

[global getCR3]
getCR3:
	mov eax, cr3
	ret

[global getESP]
getESP:
	mov eax, esp
	add eax, 4
	ret

[global getEBP]
getEBP:
	mov eax, ebp
	ret

[global getEIP]
getEIP:
	mov eax, [esp]
	ret
	;pop eax
	;jmp eax ;hack because we're popping
