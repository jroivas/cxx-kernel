set(LIBC_FILES
    syscall.cpp
    syscall_fileio.cpp
    thread.cpp
    )
add_library(libc
    ${LIBC_FILES}
    )
