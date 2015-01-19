if (APP STREQUAL "python")
    set(PYTHON_VERSION 3.4.2)
    set(PYTHON_PATH "python/Python-${PYTHON_VERSION}")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=sign-compare -Wno-sign-compare")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=missing-field-initializers -Wno-missing-field-initializers")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=unused-but-set-variable -Wno-unused-but-set-variable")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=unused-parameter -Wno-unused-parameter")

    include_directories(${PYTHON_PATH}/Include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/python)

    set(PYTHON_PARSER_FILES
        ${PYTHON_PATH}/Parser/acceler.c
        ${PYTHON_PATH}/Parser/grammar1.c
        ${PYTHON_PATH}/Parser/listnode.c
        ${PYTHON_PATH}/Parser/node.c
        ${PYTHON_PATH}/Parser/parser.c
        ${PYTHON_PATH}/Parser/bitset.c
        ${PYTHON_PATH}/Parser/metagrammar.c
        ${PYTHON_PATH}/Parser/firstsets.c
        ${PYTHON_PATH}/Parser/grammar.c
        ${PYTHON_PATH}/Parser/pgen.c
        ${PYTHON_PATH}/Parser/myreadline.c
        ${PYTHON_PATH}/Parser/parsetok.c
        ${PYTHON_PATH}/Parser/tokenizer.c
        )

    set(APP_FILES
        python/python.c
        ${PYTHON_PATH}/Modules/python.c
        ${PYTHON_PARSER_FILES}
        )

elseif (NOT DEFINED APP OR APP STREQUAL "hello" OR APP STREQUAL "" )
    set(APP_FILES
        hello/hello.c
        )
endif()

add_library(app
    ${APP_FILES}
    )
