TASK_MAGIC equ 0x42

[global changeProcess]
changeProcess:
	cli

	mov ebp, esp
	mov edx, [ebp+16]  ;pagetable
	mov eax, [ebp+12]  ;stack
	mov ebx, [ebp+8]   ;base
	mov ecx, [ebp+4]   ;eip

	mov cr3, edx

	mov ebp, ebx
	mov esp, eax

	mov eax, TASK_MAGIC

	sti
	jmp ecx


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
	pop eax
	jmp eax ;hack because we're popping
	; push eax
 	; ret
