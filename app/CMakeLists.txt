if (APP STREQUAL "python")
    set(PYTHON_VERSION 3.4.2)
    set(PYTHON_PATH "python/Python-${PYTHON_VERSION}")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=sign-compare -Wno-sign-compare")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=missing-field-initializers -Wno-missing-field-initializers")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=unused-but-set-variable -Wno-unused-but-set-variable")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=unused-parameter -Wno-unused-parameter")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=uninitialized -Wno-uninitialized")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=implicit-function-declaration -Wno-implicit-function-declaration")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error")

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

    set(PYTHON_OBJECTS_FILES
        ${PYTHON_PATH}/Objects/abstract.c
        ${PYTHON_PATH}/Objects/accu.c
        ${PYTHON_PATH}/Objects/boolobject.c
        ${PYTHON_PATH}/Objects/bytes_methods.c
        ${PYTHON_PATH}/Objects/bytearrayobject.c
        ${PYTHON_PATH}/Objects/bytesobject.c
        ${PYTHON_PATH}/Objects/cellobject.c
        ${PYTHON_PATH}/Objects/classobject.c
        ${PYTHON_PATH}/Objects/codeobject.c
        ${PYTHON_PATH}/Objects/complexobject.c
        ${PYTHON_PATH}/Objects/descrobject.c
        ${PYTHON_PATH}/Objects/enumobject.c
        ${PYTHON_PATH}/Objects/exceptions.c
        ${PYTHON_PATH}/Objects/genobject.c
        ${PYTHON_PATH}/Objects/fileobject.c
        ${PYTHON_PATH}/Objects/floatobject.c
        ${PYTHON_PATH}/Objects/frameobject.c
        ${PYTHON_PATH}/Objects/funcobject.c
        ${PYTHON_PATH}/Objects/iterobject.c
        ${PYTHON_PATH}/Objects/listobject.c
        ${PYTHON_PATH}/Objects/longobject.c
        ${PYTHON_PATH}/Objects/dictobject.c
        ${PYTHON_PATH}/Objects/memoryobject.c
        ${PYTHON_PATH}/Objects/methodobject.c
        ${PYTHON_PATH}/Objects/moduleobject.c
        ${PYTHON_PATH}/Objects/namespaceobject.c
        ${PYTHON_PATH}/Objects/object.c
        ${PYTHON_PATH}/Objects/obmalloc.c
        ${PYTHON_PATH}/Objects/capsule.c
        ${PYTHON_PATH}/Objects/rangeobject.c
        ${PYTHON_PATH}/Objects/setobject.c
        ${PYTHON_PATH}/Objects/sliceobject.c
        ${PYTHON_PATH}/Objects/structseq.c
        ${PYTHON_PATH}/Objects/tupleobject.c
        ${PYTHON_PATH}/Objects/typeobject.c
        ${PYTHON_PATH}/Objects/unicodeobject.c
        ${PYTHON_PATH}/Objects/unicodectype.c
        ${PYTHON_PATH}/Objects/weakrefobject.c
        )

    set(APP_FILES
        python/python.c
        ${PYTHON_PATH}/Modules/python.c
        ${PYTHON_PARSER_FILES}
        ${PYTHON_OBJECTS_FILES}
        )

elseif (NOT DEFINED APP OR APP STREQUAL "hello" OR APP STREQUAL "" )
    set(APP_FILES
        hello/hello.c
        )
endif()

add_library(app
    ${APP_FILES}
    )
