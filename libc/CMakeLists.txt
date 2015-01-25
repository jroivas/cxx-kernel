set(LIBC_FILES
    syscall.cpp
    thread.cpp
    )
add_library(libc
    ${LIBC_FILES}
    )
