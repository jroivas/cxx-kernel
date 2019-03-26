set(ARCH_FILES
    platform.cpp
    random.cpp
    )

if (PLATFORM STREQUAL "x86")
    add_custom_command(OUTPUT smp_init.ld
        COMMAND nasm "${CMAKE_CURRENT_SOURCE_DIR}/x86/smp_init.asm" -o "${CMAKE_CURRENT_BINARY_DIR}/smp_init.bin"
        COMMAND hexdump -v -e '\"BYTE(0x\" 1/1 \"%02X\" \")\\n\"' "${CMAKE_CURRENT_BINARY_DIR}/smp_init.bin" > "${CMAKE_CURRENT_BINARY_DIR}/smp_init.ld"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/x86/smp_init.asm"
        )
    add_custom_target(smpsupport ALL
        DEPENDS smp_init.ld
        )
    set(LOADER_FILES
        x86/loader.asm
        smp_init.ld
        )
    set(ARCH_FILES
        ${ARCH_FILES}
        x86/acpix86.cpp
        x86/cpux86.cpp
        x86/bios.cpp
        x86/idtx86.cpp
        x86/kbx86.cpp
        x86/pagingx86.cpp
        x86/port.cpp
        x86/statesx86.cpp
        x86/timerx86.cpp
        x86/videox86.cpp
        x86/randomx86.cpp
        x86/x86.cpp
        )
    if (PCI)
        set(ARCH_FILES
            ${ARCH_FILES}
            x86/pcix86.cpp
            )
    endif()
    if (GRAPHICS)
        set(ARCH_FILES
            ${ARCH_FILES}
            x86/vesa.cpp
            )
    endif()
    if (STORAGE)
        set(ARCH_FILES
            ${ARCH_FILES}
            x86/atax86.cpp
            )
    endif()
    set(ARCH_LATE_FILES
        x86/taskx86.cpp
        )
    set(ARCH_SETJMP_FILES
        x86/setjmp.asm
        )
    set(ARCH_ASM_FILES
        x86/gdt.asm
        x86/idt_handlers.asm
        )
    set(ARCH_ASM_LATE_FILES
        x86/memcpy.asm
        x86/pages.asm
        x86/process.asm
        x86/hwrandom.asm
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
endif()

if (PLATFORM STREQUAL "LINUX")
    set(LOADER_FILES
        linux/loader.cpp
        )
    set(ARCH_FILES
        ${ARCH_FILES}
        linux/acpilinux.cpp
        linux/paginglinux.cpp
        linux/idtlinux.cpp
        linux/timerlinux.cpp
        linux/stateslinux.cpp
        linux/tasklinux.cpp
        )
    if (GRAPHICS)
        set(ARCH_FILES
            ${ARCH_FILES}
            linux/x11.cpp
        )
    endif()
    if (STORAGE)
        set(ARCH_FILES
            ${ARCH_FILES}
            linux/virtualdisc.cpp
        )
    endif()
    set(ARCH_LATE_FILES
        linux/videolinux.cpp
        )
endif()

if (PLATFORM STREQUAL "ARM")
    set(LOADER_FILES
        arm/loader.s
        )
    set(ARCH_FILES
        ${ARCH_FILES}
        arm/arm.cpp
        arm/pagingarm.cpp
        arm/idtarm.cpp
        arm/timerarm.cpp
        arm/statesarm.cpp
        arm/gpio.cpp
        )
    set(ARCH_LATE_FILES
        arm/videoarm.cpp
        )
    if (GRAPHICS)
        add_library(arch_asm_late STATIC
            arm/qemu/armfb.cpp
            )
    endif()
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
