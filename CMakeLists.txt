cmake_minimum_required(VERSION 2.8)

project(kernel)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/inc)
include_directories(${CMAKE_SOURCE_DIR}/arch)

set(BASE_FLAGS "-DNO_SYS_HEADERS ")
set(BASE_FLAGS "${BASE_FLAGS} -D__FREESTAND__")
set(BASE_FLAGS "${BASE_FLAGS} -ffreestanding")
set(BASE_FLAGS "${BASE_FLAGS} -fno-builtin")
set(BASE_FLAGS "${BASE_FLAGS} -Wall")
set(BASE_FLAGS "${BASE_FLAGS} -Werror")
set(BASE_FLAGS "${BASE_FLAGS} -Wextra")
set(BASE_FLAGS "${BASE_FLAGS} -fomit-frame-pointer")

set(CMAKE_CXX_FLAGS "${BASE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-builtin")

set(CMAKE_C_FLAGS "${BASE_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdinc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdlib")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector")
find_program(GCC_LD ld REQUIRED)


set(ARCH_ERROR_MSG "Plase define ARCH to x86, ARM or LINUX (example: -DARCH=x86)")
if (NOT DEFINED ARCH)
    message(FATAL_ERROR ${ARCH_ERROR_MSG})
endif()
if (ARCH STREQUAL "x86")
    set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
    enable_language(ASM_NASM)
    set(CMAKE_ASM-NASM_COMPILER_ARG1 "-f elf32")
    include_directories(${CMAKE_SOURCE_DIR}/3rdparty/my_x86emu)
    add_definitions(-DARCH_x86)
    set(LINKFLAGS "-nostdlib")
    set(LINKFLAGS ${LINKFLAGS} -melf_i386)
    set(LINKFLAGS ${LINKFLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/link_x86.ld)
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

elseif (ARCH STREQUAL "ARM")
    enable_language(ASM-ATT)
    add_definitions(-DARCH_ARM)
    #set(LINKFLAGS ${LINKFLAGS} -marmelf)
    set(LINKFLAGS ${LINKFLAGS} -nostdlib)
    set(LINKFLAGS ${LINKFLAGS} --no-undefined)
    set(LINKFLAGS ${LINKFLAGS} -nostdlib)
    set(LINKFLAGS ${LINKFLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/link_armqemu.ld)
    set(CROSS_COMPILE $(HOME)/bin/arm-2008q3/bin/arm-none-eabi-)
    set(CMAKE_C_COMPILER "${CROSS_COMPILE}gcc")
    set(CMAKE_CXX_COMPILER "${CROSS_COMPILE}g++")
    set(GCC_LD "${CROSS_COMPILE}ld")
    set(CMAKE_ASM-ATT_COMPILER "${CROSS_COMPILE}as")
    add_definitions(-DPLATFORM_qemu=1)
elseif (ARCH STREQUAL "LINUX")
    add_definitions(-DARCH_LINUX)
    set(GCC_LD g++)
else()
    message(FATAL_ERROR ${ARCH_ERROR_MSG})
endif()

#set(CMAKE_LINKER "${GCC_LD}")
#set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
#set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <LINK_FLAGS> <OBJECTS> -o <TARGET>")
add_subdirectory(src)
add_subdirectory(arch)
add_subdirectory(3rdparty)


set(CMAKE_CXX_LINKER_PREFERENCE_PROPAGATES 0)
set(CMAKE_LINKER "${GCC_LD}")

#set_target_properties(kernel.bin PROPERTIES LINKER_LANGUAGE C PREFIX "" SUFFIX "" LINK_FLAGS "${LINKFLAGS}")
#set_target_properties(kernel.bin PROPERTIES LINKER_LANGUAGE LD PREFIX "" SUFFIX "" LINK_FLAGS "${LINKFLAGS}")

set(extra_pre "")
set(extra_mid "")
set(extra_late "")
set(emu "")
set(extra_deps "")
set(extra_libs "")
set(extra_loader "")

if (ARCH STREQUAL "x86")
    set(extra_pre $<TARGET_FILE:arch_asm>)
    set(extra_mid $<TARGET_FILE:arch_setjmp>)
    set(extra_late $<TARGET_FILE:arch_asm_late>)
    set(emu $<TARGET_FILE:x86emu>)
    set(extra_deps arch_asm arch_setjmp arch_asm_late x86emu
    )
elseif(ARCH STREQUAL "LINUX")
    set(extra_libs -lc -lpthread -lX11)
elseif(ARCH STREQUAL "ARM")
    set(extra_late $<TARGET_FILE:arch_asm_late>)
    set(extra_deps arch_asm_late)
    #FIXME HACK: For some reason linking lib does not work with arm
    set(extra_loader ${CMAKE_BINARY_DIR}/arch/CMakeFiles/loader.dir/arm/loader.s.o)
endif()

add_custom_command(OUTPUT kernel.bin
    COMMAND ${GCC_LD} ${LINKFLAGS} -o kernel.bin $<TARGET_FILE:loader> ${extra_loader} $<TARGET_FILE:core> $<TARGET_FILE:mm>
        $<TARGET_FILE:tables> $<TARGET_FILE:kb> $<TARGET_FILE:graphics> $<TARGET_FILE:coretools> $<TARGET_FILE:bits> $<TARGET_FILE:font>
        $<TARGET_FILE:storage> $<TARGET_FILE:task> $<TARGET_FILE:base> ${extra_pre}
        ${extra_mid} $<TARGET_FILE:arch> $<TARGET_FILE:bits> $<TARGET_FILE:arch_late>
        $<TARGET_FILE:task> ${extra_late} ${emu}
        ${extra_mid} $<TARGET_FILE:coretools> $<TARGET_FILE:font_3pp>
        ${extra_libs}
    DEPENDS kernel
    loader
    core
    mm
    tables
    kb
    graphics
    coretools
    bits
    font
    font_3pp
    task
    storage
    base
    arch
    arch_late
    ${extra_deps}
    )

add_custom_target(kernel ALL
    DEPENDS kernel.bin
    )

if (ARCH STREQUAL "x86")
    add_custom_command(OUTPUT kernel.iso
        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/isofiles/boot/grub
        COMMAND cp -f kernel.bin ${CMAKE_CURRENT_BINARY_DIR}/isofiles/boot/kernel
        COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/stage2_eltorito ${CMAKE_CURRENT_BINARY_DIR}/isofiles/boot/grub
        COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/menu.lst ${CMAKE_CURRENT_BINARY_DIR}/isofiles/boot/grub
        COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/kernel.iso
        COMMAND genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -quiet -boot-load-size 4 -boot-info-table -o ${CMAKE_CURRENT_BINARY_DIR}/kernel.iso ${CMAKE_CURRENT_BINARY_DIR}/isofiles
        DEPENDS menu.lst
        DEPENDS stage2_eltorito
        DEPENDS kernel.bin
        )
    add_custom_target(kernel_image ALL
        DEPENDS kernel.iso
        )
    add_custom_target(run
        COMMAND kvm -serial mon:stdio -cdrom ${CMAKE_CURRENT_BINARY_DIR}/kernel.iso
        DEPENDS kernel.iso
        )
endif()
if (ARCH STREQUAL "ARM")
    add_custom_target(run
        COMMAND qemu-system-arm -serial mon:stdio -M versatilepb -m 128M -kernel kernel.bin
        DEPENDS kernel.bin
        )
endif()
