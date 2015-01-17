add_subdirectory(font)

if (ARCH STREQUAL "x86")
    add_subdirectory(my_x86emu)
endif()
