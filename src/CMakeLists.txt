set(CORE_FILES
    kernel.cpp
    video.cpp
    main.cpp
    cxa.cpp
    local.cpp
    mutex.cpp
    operators.cpp
    timer.cpp
    )

set(MM_FILES
    mm.cpp
    paging.cpp
    )

set(TABLES_FILES
    gdt.cpp
    idt.cpp
    )

set(KB_FILES
    kb.cpp
    )

set(GRAPHICS_FILES
    fb.cpp
    )

set(BITS_FILES
    bits.cpp
    )

set(CORETOOLS_FILES
    math.cpp
    states.cpp
    setjmp.cpp
    string.cpp
    list.cpp
    )

set(FONT_FILES
    font.cpp
    )

set(STORAGE_FILES
    ata.cpp
    pci.cpp
    storage.cpp
    )

set(TASK_FILES
    task.cpp
    processmanager.cpp
    )

set(BASE_FILES
    syscall.cpp
    )

add_library(core STATIC ${CORE_FILES})
add_library(base STATIC ${BASE_FILES})
add_library(mm STATIC ${MM_FILES})
add_library(tables STATIC ${TABLES_FILES})
add_library(kb STATIC ${KB_FILES})
add_library(graphics STATIC ${GRAPHICS_FILES})
add_library(bits STATIC ${BITS_FILES})
add_library(coretools STATIC ${CORETOOLS_FILES})
add_library(font STATIC ${FONT_FILES})
add_library(storage STATIC ${STORAGE_FILES})
add_library(task STATIC ${TASK_FILES})
