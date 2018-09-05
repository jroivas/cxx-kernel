[extern cpuid_has_feature]
[global getHwRandom]
getHwRandom:
    rdrand eax
    ret

[global hasHwRandom]
hasHwRandom:
    mov eax, 23
    push eax
    call cpuid_has_feature
    add esp, 4
    ret
