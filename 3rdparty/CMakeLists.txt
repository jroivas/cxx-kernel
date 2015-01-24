add_subdirectory(font)

add_subdirectory(musl)
if (PLATFORM STREQUAL "x86")
    add_subdirectory(my_x86emu)
endif()
