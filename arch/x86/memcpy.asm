[global cpuid_has_feature]
cpuid_has_feature:
    push ebp
    mov ebp, esp
    push ebx
    push ecx
    push edx
    mov eax, 1
    xor ecx, ecx
    cpuid
    mov eax, ecx
    mov ecx, [esp + 16]
    shr eax, cl
    and eax, 1
    pop edx
    pop ecx
    pop ebx
    leave
    ret

[global mmx_has]
mmx_has:
    mov eax, 23
    push eax
    call cpuid_has_feature
    add esp, 4
    ret

[global sse_has]
sse_has:
    mov eax, 25
    push eax
    call cpuid_has_feature
    add esp, 4
    ret

[global sse2_has]
sse2_has:
    mov eax, 26
    push eax
    call cpuid_has_feature
    add esp, 4
    ret

[global mmx_memcpy]
mmx_memcpy:
    push ebp
    mov ebp, esp
    mov edi, [esp+4]
    mov esi, [esp+8]
    mov ecx, [esp+12]
    mov ebx, ecx
    shr ecx, 6
    push ecx
    shl ecx, 6
    sub ebx, ecx
    pop ecx
    test ecx, ecx
    jz __mmx_memcpy_rest

__mmx_memcpy_loop:
    movq mm0, [esi]
    movq mm1, [esi+8]
    movq mm2, [esi+16]
    movq mm3, [esi+24]
    movq mm4, [esi+32]
    movq mm5, [esi+40]
    movq mm6, [esi+48]
    movq mm7, [esi+56]
    movq [edi], mm0
    movq [edi+8], mm1
    movq [edi+16], mm2
    movq [edi+24], mm3
    movq [edi+32], mm4
    movq [edi+40], mm5
    movq [edi+48], mm6
    movq [edi+56], mm7

    add esi, 64
    add edi, 64
    ;add esi, 32
    ;add edi, 32
    dec ecx
    jnz __mmx_memcpy_loop

__mmx_memcpy_rest:
    test ebx, ebx
    jz __mmx_memcpy_end
    mov ecx, ebx
    rep movsb
    
__mmx_memcpy_end:
    emms
    ;pop ebp
    leave
    ret

[extern memcpy]
[global memcpy_opt]
__memcpy_internal_has_mmx:
    dd 0
memcpy_opt:
    push ebp
    mov ebp, esp
    sub esp, 0x10
    mov eax, [__memcpy_internal_has_mmx]
    cmp eax, 1
    je __memcpy_mmx_has
    cmp eax, 0
    jne __memcpy_trad

__memcpy_mmx_update_info:
    call mmx_has
    cmp eax, 1
    jne __memcpy_trad_update
    mov dword [__memcpy_internal_has_mmx], 1

__memcpy_mmx_has:
    ;mov ebp, esp
    ;push dword [ebp+12]
    ;push dword [ebp+8]
    ;push dword [ebp+4]
    push dword [ebp+16]
    push dword [ebp+12]
    push dword [ebp+8]
    call mmx_memcpy
    add esp, 0x10
    ;pop ebp
    ;add esp, 12
    ;add esp, 0x10
    leave
    ret

__memcpy_trad_update:
    mov dword [__memcpy_internal_has_mmx], 2

__memcpy_trad:
    ;push ebp
    ;mov ebp, esp
    push dword [ebp+16]
    push dword [ebp+12]
    push dword [ebp+8]
    call memcpy
    add esp, 0x10
    leave
    ret
