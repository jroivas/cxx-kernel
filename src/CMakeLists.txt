set(CORE_FILES
    kernel.cpp
    video.cpp
    main.cpp
    cxa.cpp
    local.cpp
    mutex.cpp
    operators.cpp
    )

set(MM_FILES
    mm.cpp
    paging.cpp
    )

set(TABLES_FILES
    gdt.cpp
    string.cpp
    idt.cpp
    )

set(KBFB_FILES
    timer.cpp
    kb.cpp
    fb.cpp
    )

set(BITS_FILES
    bits.cpp
    )
set(TMP1_FILES
    math.cpp
    states.cpp
    setjmp.cpp
    )

set(TMP2_FILES
    font.cpp
    )

set(TMP3_FILES
    ata.cpp
    pci.cpp
    storage.cpp
    )

set(TMP4_FILES
    task.cpp
    list.cpp
    processmanager.cpp
    )

set(BASE_FILES
    syscall.cpp
    )

add_library(core STATIC ${CORE_FILES})
add_library(base STATIC ${BASE_FILES})
add_library(mm STATIC ${MM_FILES})
add_library(tables STATIC ${TABLES_FILES})
add_library(kbfb STATIC ${KBFB_FILES})
add_library(bits STATIC ${BITS_FILES})
add_library(tmp1 STATIC ${TMP1_FILES})
add_library(tmp2 STATIC ${TMP2_FILES})
add_library(tmp3 STATIC ${TMP3_FILES})
add_library(tmp4 STATIC ${TMP4_FILES})
