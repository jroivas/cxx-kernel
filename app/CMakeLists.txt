if (APP STREQUAL "python")
    set(PYTHON_VERSION 3.4.2)
    set(PYTHON_PATH "python/Python-${PYTHON_VERSION}")

    include_directories(${PYTHON_PATH}/Include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/python)

    add_subdirectory(python)

    set(APP_FILES
        python/python.c
        ${PYTHON_PATH}/Modules/python.c
        )

elseif (NOT DEFINED APP OR APP STREQUAL "hello" OR APP STREQUAL "" )
    set(APP_FILES
        hello/hello.c
        )
endif()

add_library(app
    ${APP_FILES}
    )
