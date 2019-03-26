if (APP STREQUAL "python")
    set(PYTHON_MAJOR 3)
    set(PYTHON_MINOR 4)
    set(PYTHON_PATCH 2)
    set(PYTHON_VERSION "${PYTHON_MAJOR}.${PYTHON_MINOR}.${PYTHON_PATCH}")
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

    set(PYTHON_FILES
        ${PYTHON_PATH}/Python/_warnings.c
        ${PYTHON_PATH}/Python/Python-ast.c
        ${PYTHON_PATH}/Python/asdl.c
        ${PYTHON_PATH}/Python/ast.c
        ${PYTHON_PATH}/Python/bltinmodule.c
        ${PYTHON_PATH}/Python/ceval.c
        ${PYTHON_PATH}/Python/compile.c
        ${PYTHON_PATH}/Python/codecs.c
        ${PYTHON_PATH}/Python/dynamic_annotations.c
        ${PYTHON_PATH}/Python/errors.c
        ${PYTHON_PATH}/Python/frozenmain.c
        ${PYTHON_PATH}/Python/future.c
        ${PYTHON_PATH}/Python/getargs.c
        ${PYTHON_PATH}/Python/getcompiler.c
        ${PYTHON_PATH}/Python/getcopyright.c
        ${PYTHON_PATH}/Python/getplatform.c
        ${PYTHON_PATH}/Python/getversion.c
        ${PYTHON_PATH}/Python/graminit.c
        ${PYTHON_PATH}/Python/import.c
        ${PYTHON_PATH}/Python/importdl.c
        ${PYTHON_PATH}/Python/marshal.c
        ${PYTHON_PATH}/Python/modsupport.c
        ${PYTHON_PATH}/Python/mystrtoul.c
        ${PYTHON_PATH}/Python/mysnprintf.c
        ${PYTHON_PATH}/Python/peephole.c
        ${PYTHON_PATH}/Python/pyarena.c
        ${PYTHON_PATH}/Python/pyctype.c
        ${PYTHON_PATH}/Python/pyfpe.c
        ${PYTHON_PATH}/Python/pyhash.c
        ${PYTHON_PATH}/Python/pymath.c
        ${PYTHON_PATH}/Python/pystate.c
        ${PYTHON_PATH}/Python/pythonrun.c
        ${PYTHON_PATH}/Python/pytime.c
        ${PYTHON_PATH}/Python/random.c
        ${PYTHON_PATH}/Python/structmember.c
        ${PYTHON_PATH}/Python/symtable.c
        ${PYTHON_PATH}/Python/sysmodule.c
        ${PYTHON_PATH}/Python/traceback.c
        ${PYTHON_PATH}/Python/getopt.c
        ${PYTHON_PATH}/Python/pystrcmp.c
        ${PYTHON_PATH}/Python/pystrtod.c
        ${PYTHON_PATH}/Python/dtoa.c
        ${PYTHON_PATH}/Python/formatter_unicode.c
        ${PYTHON_PATH}/Python/fileutils.c
        ${PYTHON_PATH}/Python/dynload_shlib.c # FIXME
        ${PYTHON_PATH}/Python/thread.c
        )

    set(PYTHON_MODULE
        #${PYTHON_PATH}/Modules/config.c
        python/config.c
        ${PYTHON_PATH}/Modules/getpath.c
        ${PYTHON_PATH}/Modules/main.c
        ${PYTHON_PATH}/Modules/gcmodule.c
        )

    set(PYTHON_MODOBJS
        ${PYTHON_PATH}/Modules/_threadmodule.c
        ${PYTHON_PATH}/Modules/signalmodule.c
        ${PYTHON_PATH}/Modules/posixmodule.c
        ${PYTHON_PATH}/Modules/errnomodule.c
        ${PYTHON_PATH}/Modules/pwdmodule.c
        ${PYTHON_PATH}/Modules/_sre.c
        ${PYTHON_PATH}/Modules/_codecsmodule.c
        ${PYTHON_PATH}/Modules/_weakref.c
        ${PYTHON_PATH}/Modules/_functoolsmodule.c
        ${PYTHON_PATH}/Modules/_operator.c
        ${PYTHON_PATH}/Modules/_collectionsmodule.c
        ${PYTHON_PATH}/Modules/itertoolsmodule.c
        ${PYTHON_PATH}/Modules/atexitmodule.c
        ${PYTHON_PATH}/Modules/_stat.c
        ${PYTHON_PATH}/Modules/_localemodule.c
        ${PYTHON_PATH}/Modules/_io/_iomodule.c
        ${PYTHON_PATH}/Modules/_io/iobase.c
        ${PYTHON_PATH}/Modules/_io/fileio.c
        ${PYTHON_PATH}/Modules/_io/bytesio.c
        ${PYTHON_PATH}/Modules/_io/bufferedio.c
        ${PYTHON_PATH}/Modules/_io/textio.c
        ${PYTHON_PATH}/Modules/_io/stringio.c
        ${PYTHON_PATH}/Modules/zipimport.c
        ${PYTHON_PATH}/Modules/faulthandler.c
        ${PYTHON_PATH}/Modules/_tracemalloc.c
        ${PYTHON_PATH}/Modules/hashtable.c
        ${PYTHON_PATH}/Modules/symtablemodule.c
        ${PYTHON_PATH}/Modules/xxsubtype.c
        )

    set(APP_FILES
        python/python.c
        ${PYTHON_PATH}/Modules/python.c
        ${PYTHON_PATH}/Modules/getbuildinfo.c
        ${PYTHON_PARSER_FILES}
        ${PYTHON_OBJECTS_FILES}
        ${PYTHON_PATH}/Python/frozen.c
        ${PYTHON_FILES}
        ${PYTHON_MODULE}
        ${PYTHON_MODOBJS}
        )

elseif (APP STREQUAL "randomfile")
    set(APP_FILES
        randomfile/randomfile.c
        )
elseif (APP STREQUAL "readfile")
    set(APP_FILES
        readfile/readfile.c
        )
elseif (NOT DEFINED APP OR APP STREQUAL "hello" OR APP STREQUAL "" )
    set(APP_FILES
        hello/hello.c
        )
endif()

add_definitions(
    -DSOABI="cpython-${PYTHON_MAJOR}${PYTHON_MINOR}m"
    )

add_library(app
    ${APP_FILES}
    )

add_custom_command(TARGET app POST_BUILD
    COMMAND "${CROSS_COMPILE}objcopy" --redefine-sym main=app_main ${CMAKE_CURRENT_BINARY_DIR}/libapp.a
    )
