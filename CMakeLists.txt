cmake_minimum_required(VERSION 2.8)

project(kernel)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/inc)
if (STORAGE)
    include_directories(${CMAKE_SOURCE_DIR}/inc/fs)
endif()
include_directories(${CMAKE_SOURCE_DIR}/arch)
include_directories(${CMAKE_SOURCE_DIR}/app)
if (NOT PLATFORM STREQUAL "LINUX")
    include_directories(${CMAKE_SOURCE_DIR}/libc)
endif()


set(BASE_FLAGS "-DNO_SYS_HEADERS ")
set(BASE_FLAGS "${BASE_FLAGS} -D__FREESTAND__")
set(BASE_FLAGS "${BASE_FLAGS} -ffreestanding")
set(BASE_FLAGS "${BASE_FLAGS} -nostdlib")
set(BASE_FLAGS "${BASE_FLAGS} -lgcc")
#set(BASE_FLAGS "${BASE_FLAGS} -fno-builtin")
set(BASE_FLAGS "${BASE_FLAGS} -Wall")
set(BASE_FLAGS "${BASE_FLAGS} -Werror")
set(BASE_FLAGS "${BASE_FLAGS} -Wextra")
set(BASE_FLAGS "${BASE_FLAGS} -fomit-frame-pointer")

set(CMAKE_CXX_FLAGS "${BASE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-builtin")

set(CMAKE_C_FLAGS "${BASE_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdlib")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -save-temps")
find_program(GCC_LD ld REQUIRED)

add_definitions(-DUSE_CUSTOM_STRING)
add_definitions(-DSTD_STRING_TYPE=String)

set(PLATFORM_ERROR_MSG "Plase define PLATFORM to x86, ARM or LINUX (example: -DPLATFORM=x86)")
set(ARCH_ERROR_MSG "Plase define ARCH (example: -DARCH=i386)")
if (NOT DEFINED PLATFORM)
    message(FATAL_ERROR ${PLATFORM_ERROR_MSG})
endif()
if (NOT DEFINED ARCH)
    message(FATAL_ERROR ${ARCH_ERROR_MSG})
endif()
if (PLATFORM STREQUAL "x86")
    set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
    enable_language(ASM_NASM)
    set(CMAKE_ASM-NASM_COMPILER_ARG1 "-f elf32")
    include_directories(${CMAKE_SOURCE_DIR}/3rdparty/my_x86emu)
    add_definitions(-DARCH_x86)
    set(LINKFLAGS "-nostdlib")
    set(LINKFLAGS ${LINKFLAGS} "-ffreestanding")
    set(LINKFLAGS ${LINKFLAGS} "-lgcc")
    set(LINKFLAGS ${LINKFLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/link_x86.ld)
    set(CROSS_COMPILE i686-elf-)
    set(CMAKE_C_COMPILER "${CROSS_COMPILE}gcc")
    set(CMAKE_CXX_COMPILER "${CROSS_COMPILE}g++")
    set(GCC_LD "${CROSS_COMPILE}gcc")
elseif (PLATFORM STREQUAL "ARM")
    enable_language(ASM-ATT)
    add_definitions(-DARCH_ARM)
    set(LINKFLAGS ${LINKFLAGS} -nostdlib)
    set(LINKFLAGS ${LINKFLAGS} --no-undefined)
    set(LINKFLAGS ${LINKFLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/link_armqemu.ld)
    set(CROSS_COMPILE arm-none-eabi-)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdinc")
    set(CMAKE_C_COMPILER "${CROSS_COMPILE}gcc")
    set(CMAKE_CXX_COMPILER "${CROSS_COMPILE}g++")
    set(GCC_LD "${CROSS_COMPILE}ld")
    set(CMAKE_ASM-ATT_COMPILER "${CROSS_COMPILE}as")
    add_definitions(-DPLATFORM_qemu=1)
elseif (PLATFORM STREQUAL "LINUX")
    add_definitions(-DARCH_LINUX)
    set(GCC_LD g++)
else()
    message(FATAL_ERROR ${PLATFORM_ERROR_MSG})
endif()

set(CMAKE_CXX_LINKER_PREFERENCE_PROPAGATES 0)
set(CMAKE_LINKER "${GCC_LD}")

set(extra_pre "")
set(extra_mid "")
set(extra_late "")
set(emu "")
set(extra_deps "")
set(extra_libs "")
set(extra_loader "")

if (PLATFORM STREQUAL "x86")
    set(extra_pre $<TARGET_FILE:arch_asm>)
    set(extra_mid $<TARGET_FILE:arch_setjmp>)
    set(extra_late $<TARGET_FILE:arch_asm_late>)
    set(emu $<TARGET_FILE:x86emu>)
    set(extra_deps arch_asm arch_setjmp arch_asm_late x86emu
    )
elseif(PLATFORM STREQUAL "LINUX")
    set(extra_libs -lc -lpthread -lX11)
elseif(PLATFORM STREQUAL "ARM")
    if (GRAPHICS)
        set(extra_late $<TARGET_FILE:arch_asm_late>)
        set(extra_deps arch_asm_late)
    endif()
    #FIXME HACK: For some reason linking lib does not work with arm
    set(extra_loader ${CMAKE_BINARY_DIR}/arch/CMakeFiles/loader.dir/arm/loader.s.o)
endif()

if (GRAPHICS)
    set(graphics_files $<TARGET_FILE:graphics>)
    set(graphics_deps graphics)
    add_definitions(-DFEATURE_GRAPHICS)
else()
    set(graphics_files "")
    set(graphics_deps "")
endif()


if (STORAGE)
    set(storage_files $<TARGET_FILE:storage> $<TARGET_FILE:fs>)
    set(storage_deps storage fs)
    add_definitions(-DFEATURE_STORAGE)
    set(PCI True)
else()
    set(storage_files "")
    set(storage_deps "")
endif()

if (PCI)
    add_definitions(-DFEATURE_PCI)
endif()

if (PLATFORM STREQUAL "LINUX")
    set(libc_files "")
    set(libc_deps "")
else()
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/musl/musl/arch/generic")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/musl/musl/arch/${ARCH}")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/musl/musl/src/internal")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/musl/musl/include")
    add_subdirectory(libc)
    set(extra_src
        )
    set(libc_files
        $<TARGET_FILE:musl>
        $<TARGET_FILE:libc_init>
        $<TARGET_FILE:coretools>
        $<TARGET_FILE:libc>
        )
    set(libc_deps libc musl coretools)
endif()

add_subdirectory(src)
add_subdirectory(arch)
add_subdirectory(3rdparty)
add_subdirectory(app)

add_custom_command(OUTPUT kernel.bin
    COMMAND ${GCC_LD} ${LINKFLAGS} -o kernel.bin
        $<TARGET_FILE:loader>
        ${extra_loader}
        $<TARGET_FILE:core>
        $<TARGET_FILE:mm>
        $<TARGET_FILE:tables>
        $<TARGET_FILE:kb>
        ${graphics_files}
        $<TARGET_FILE:coretools>
        $<TARGET_FILE:bits>
        $<TARGET_FILE:font>
        ${storage_files}
        $<TARGET_FILE:task>
        $<TARGET_FILE:base>
        ${extra_pre}
        ${extra_mid}
        $<TARGET_FILE:arch>
        $<TARGET_FILE:bits>
        $<TARGET_FILE:arch_late>
        $<TARGET_FILE:task>
        ${extra_late}
        ${emu}
        ${extra_mid}
        $<TARGET_FILE:coretools>
        $<TARGET_FILE:font_3pp>
        ${extra_libs}
        $<TARGET_FILE:app>
        ${libc_files}
    DEPENDS kernel
    loader
    core
    mm
    tables
    kb
    ${graphics_deps}
    coretools
    bits
    font
    font_3pp
    task
    ${storage_deps}
    base
    arch
    arch_late
    ${extra_deps}
    app
    ${libc_deps}
    )

add_custom_target(kernel ALL
    DEPENDS kernel.bin
    )

if (PLATFORM STREQUAL "x86")
    add_custom_target(run
        COMMAND kvm -serial mon:stdio -kernel ${CMAKE_CURRENT_BINARY_DIR}/kernel.bin
        DEPENDS kernel.bin
        )
endif()
if (PLATFORM STREQUAL "ARM")
    add_custom_target(run
        COMMAND qemu-system-arm -serial mon:stdio -M versatilepb -m 128M -kernel kernel.bin
        DEPENDS kernel.bin
        )
endif()
if (PLATFORM STREQUAL "LINUX")
    add_custom_target(run
        COMMAND ./kernel.bin
        DEPENDS kernel.bin
        )
endif()
