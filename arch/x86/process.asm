[global saveProcess]
saveProcess:
    mov eax, [esp+4] ;save stack

    mov ecx, esp
    add ecx, 4

    mov edx, [esp]

    mov [eax], edi
    mov [eax+4], esi
    mov [eax+8], ebx
    mov [eax+12], ebp
    mov [eax+16], ecx
    mov [eax+20], edx

    mov byte [eax+24], 0
    mov edx, cr0
    and edx, 0x8
    cmp edx, 0x8
    je .saveProcess_nofpu
    mov byte [eax+24], 1

.saveProcess_nofpu:

    xor eax, eax
    ret

[global restoreProcess]
restoreProcess:
    mov ecx, [esp+4] ;lock
    mov eax, [esp+8] ;save stack

    cmp byte [eax+24], 1
    mov edx, cr0
    je .restoreProcess_usesfpu
    or eax, 0x8
    jmp .restoreProcess_fpu_over
.restoreProcess_usesfpu:
    and edx, 0xfffffff7
.restoreProcess_fpu_over:
    mov cr0, edx

    mov edi, [eax]
    mov esi, [eax+4]
    mov ebx, [eax+8]
    mov ebp, [eax+12]
    mov esp, [eax+16]
    mov edx, [eax+20]

    cmp ecx, 0
    jz .restoreProcess_no_lock
.restoreProcess_retry_lock:
    ;mov eax, 1
    ;mov edi, 0
    ;lock cmpxchg [ecx], edi
    ;jnz .restoreProcess_retry_lock

    mov dword [ecx], 0
.restoreProcess_no_lock:

    mov al, 0x20
    out 0x20, al

    xor eax,eax
    inc eax

    sti
    push edx
    ret

[global restoreUserProcess]
restoreUserProcess:
    mov ecx, [esp+4] ;lock
    mov eax, [esp+8] ;save stack

    cmp byte [eax+24], 1
    mov edx, cr0
    je .restoreProcess_usesfpu
    or eax, 0x8
    jmp .restoreProcess_fpu_over
.restoreProcess_usesfpu:
    and edx, 0xfffffff7
.restoreProcess_fpu_over:
    mov cr0, edx

    mov edi, [eax]
    mov esi, [eax+4]
    mov ebx, [eax+8]
    mov ebp, [eax+12]
    mov esp, [eax+16]
    mov edx, [eax+20]

    cmp ecx, 0
    jz .restoreProcess_no_lock
.restoreProcess_retry_lock:
    ;mov eax, 1
    ;mov edi, 0
    ;lock cmpxchg [ecx], edi
    ;jnz .restoreProcess_retry_lock

    mov dword [ecx], 0
.restoreProcess_no_lock:

    mov al, 0x20
    out 0x20, al

    xor eax,eax
    inc eax

    sti
    push edx
    ret

[global changeProcess]
changeProcess:
    ; FIXME
    mov bx, 0x10
    mov fs, bx
    mov gs, bx

    mov ebp, esp
    mov ecx, [ebp+4]   ;lock
    mov ebx, [ebp+12]  ;stack

    mov esi, [ebp+20]  ;killer
    mov edx, [ebp+16]  ;parm
    mov eax, [ebp+8]   ;eip

    ; Change stack
    cmp ebx,0
    jz .changeProcess_no_stack
    mov esp, ebx
.changeProcess_no_stack:

    ; Unlocking
    cmp ecx, 0
    jz .changeProcess_no_lock
.restoreProcess_retry_lock:
    ;mov eax, 1
    ;mov edi, 0
    ;lock cmpxchg [ecx], edi
    ;jnz .restoreProcess_retry_lock

    mov dword [ecx], 0
.changeProcess_no_lock:

    ;mov esi, [ebp+20]  ;killer
    ;mov edx, [ebp+16]  ;parm
    ;mov eax, [ebp+8]   ;eip

    mov al, 0x20
    out 0x20, al

    mov eax, [ebp+8]   ;eip

    push edx
    push esi ; Killer, when this thread exit

    xor ebp, ebp
    sti

    jmp eax
    ret

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

[global getEIP2]
getEIP2:
    mov eax, [esp]
    ret
