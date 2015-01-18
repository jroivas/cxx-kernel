add_subdirectory(font)

add_subdirectory(osdevlibc)
if (ARCH STREQUAL "x86")
    add_subdirectory(my_x86emu)
endif()
