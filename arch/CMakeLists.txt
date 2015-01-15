set(ARCH_FILES
    platform.cpp
    )

if (ARCH STREQUAL "x86")
    set(LOADER_FILES
        x86/loader.asm
        )
    set(ARCH_FILES
        ${ARCH_FILES}
        x86/atax86.cpp
        x86/bios.cpp
        x86/idtx86.cpp
        x86/kbx86.cpp
        x86/pagingx86.cpp
        x86/pcix86.cpp
        x86/port.cpp
        x86/statesx86.cpp
        x86/timerx86.cpp
        x86/vesa.cpp
        x86/videox86.cpp
        x86/x86.cpp
        )
    set(ARCH_LATE_FILES
        x86/taskx86.cpp
        )
    set(ARCH_SETJMP_FILES
        x86/setjmp.asm
        )
    set(ARCH_ASM_FILES
        x86/gdt.asm
        x86/idt_handlers.asm
        x86/memcpy.asm
        )
    set(ARCH_ASM_LATE_FILES
        x86/pages.asm
        x86/process.asm
        )
endif()

add_library(loader STATIC
    ${LOADER_FILES}
    )
add_library(arch STATIC
    ${ARCH_FILES}
    )
add_library(arch_late STATIC
    ${ARCH_LATE_FILES}
    )
add_library(arch_setjmp STATIC
    ${ARCH_SETJMP_FILES}
    )
add_library(arch_asm STATIC
    ${ARCH_ASM_FILES}
    )
add_library(arch_asm_late STATIC
    ${ARCH_ASM_LATE_FILES}
    )