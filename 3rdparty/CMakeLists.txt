add_subdirectory(font)

if (NOT PLATFORM STREQUAL "LINUX")
    add_subdirectory(musl)
endif()
if (PLATFORM STREQUAL "x86")
    add_subdirectory(my_x86emu)
endif()
