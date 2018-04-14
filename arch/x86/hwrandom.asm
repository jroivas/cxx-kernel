[global getHwRandom]
getHwRandom:
    rdrand eax
    ret
