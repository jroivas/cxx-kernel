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

    add_definitions(
        -DSOABI="cpython-${PYTHON_MAJOR}${PYTHON_MINOR}m"
        )

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

elseif (APP STREQUAL "micropython")
    set(PYTHON_MAJOR 1)
    set(PYTHON_MINOR 16)
    set(PYTHON_VERSION "${PYTHON_MAJOR}.${PYTHON_MINOR}")
    set(PYTHON_PATH "micropython/micropython-${PYTHON_VERSION}")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=unused-parameter -Wno-unused-parameter")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=sign-compare -Wno-sign-compare")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=missing-field-initializers -Wno-missing-field-initializers")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=unused-but-set-variable -Wno-unused-but-set-variable")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=unused-parameter -Wno-unused-parameter")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=uninitialized -Wno-uninitialized")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=implicit-function-declaration -Wno-implicit-function-declaration")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error")

    include_directories(${PYTHON_PATH}/py)
    include_directories(${PYTHON_PATH}/)
    include_directories(${PYTHON_PATH}/ports/unix)
    include_directories(${PYTHON_PATH}/ports/unix/variants/standard)
    include_directories(${PYTHON_PATH}/lib)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/micropython)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/micropython/build)

    set(PYTHON_CORE_FILES
	${PYTHON_PATH}/py/mpstate.c
	${PYTHON_PATH}/py/nlr.c
	${PYTHON_PATH}/py/nlrx86.c
	${PYTHON_PATH}/py/nlrx64.c
	${PYTHON_PATH}/py/nlrthumb.c
	${PYTHON_PATH}/py/nlraarch64.c
	${PYTHON_PATH}/py/nlrpowerpc.c
	${PYTHON_PATH}/py/nlrxtensa.c
	${PYTHON_PATH}/py/nlrsetjmp.c
	${PYTHON_PATH}/py/malloc.c
	${PYTHON_PATH}/py/gc.c
	${PYTHON_PATH}/py/pystack.c
	${PYTHON_PATH}/py/qstr.c
	${PYTHON_PATH}/py/vstr.c
	${PYTHON_PATH}/py/mpprint.c
	${PYTHON_PATH}/py/unicode.c
	${PYTHON_PATH}/py/mpz.c
	${PYTHON_PATH}/py/reader.c
	${PYTHON_PATH}/py/lexer.c
	${PYTHON_PATH}/py/parse.c
	${PYTHON_PATH}/py/scope.c
	${PYTHON_PATH}/py/compile.c
	${PYTHON_PATH}/py/emitcommon.c
	${PYTHON_PATH}/py/emitbc.c
	${PYTHON_PATH}/py/asmbase.c
	${PYTHON_PATH}/py/asmx64.c
	${PYTHON_PATH}/py/emitnx64.c
	${PYTHON_PATH}/py/asmx86.c
	${PYTHON_PATH}/py/emitnx86.c
	${PYTHON_PATH}/py/asmthumb.c
	${PYTHON_PATH}/py/emitnthumb.c
	${PYTHON_PATH}/py/emitinlinethumb.c
	${PYTHON_PATH}/py/asmarm.c
	${PYTHON_PATH}/py/emitnarm.c
	${PYTHON_PATH}/py/asmxtensa.c
	${PYTHON_PATH}/py/emitnxtensa.c
	${PYTHON_PATH}/py/emitinlinextensa.c
	${PYTHON_PATH}/py/emitnxtensawin.c
	${PYTHON_PATH}/py/formatfloat.c
	${PYTHON_PATH}/py/parsenumbase.c
	${PYTHON_PATH}/py/parsenum.c
	${PYTHON_PATH}/py/emitglue.c
	${PYTHON_PATH}/py/persistentcode.c
	${PYTHON_PATH}/py/runtime.c
	${PYTHON_PATH}/py/runtime_utils.c
	${PYTHON_PATH}/py/scheduler.c
	${PYTHON_PATH}/py/nativeglue.c
	${PYTHON_PATH}/py/pairheap.c
	${PYTHON_PATH}/py/ringbuf.c
	${PYTHON_PATH}/py/stackctrl.c
	${PYTHON_PATH}/py/argcheck.c
	${PYTHON_PATH}/py/warning.c
	${PYTHON_PATH}/py/profile.c
	${PYTHON_PATH}/py/map.c
	${PYTHON_PATH}/py/obj.c
	${PYTHON_PATH}/py/objarray.c
	${PYTHON_PATH}/py/objattrtuple.c
	${PYTHON_PATH}/py/objbool.c
	${PYTHON_PATH}/py/objboundmeth.c
	${PYTHON_PATH}/py/objcell.c
	${PYTHON_PATH}/py/objclosure.c
	${PYTHON_PATH}/py/objcomplex.c
	${PYTHON_PATH}/py/objdeque.c
	${PYTHON_PATH}/py/objdict.c
	${PYTHON_PATH}/py/objenumerate.c
	${PYTHON_PATH}/py/objexcept.c
	${PYTHON_PATH}/py/objfilter.c
	${PYTHON_PATH}/py/objfloat.c
	${PYTHON_PATH}/py/objfun.c
	${PYTHON_PATH}/py/objgenerator.c
	${PYTHON_PATH}/py/objgetitemiter.c
	${PYTHON_PATH}/py/objint.c
	${PYTHON_PATH}/py/objint_longlong.c
	${PYTHON_PATH}/py/objint_mpz.c
	${PYTHON_PATH}/py/objlist.c
	${PYTHON_PATH}/py/objmap.c
	${PYTHON_PATH}/py/objmodule.c
	${PYTHON_PATH}/py/objobject.c
	${PYTHON_PATH}/py/objpolyiter.c
	${PYTHON_PATH}/py/objproperty.c
	${PYTHON_PATH}/py/objnone.c
	${PYTHON_PATH}/py/objnamedtuple.c
	${PYTHON_PATH}/py/objrange.c
	${PYTHON_PATH}/py/objreversed.c
	${PYTHON_PATH}/py/objset.c
	${PYTHON_PATH}/py/objsingleton.c
	${PYTHON_PATH}/py/objslice.c
	${PYTHON_PATH}/py/objstr.c
	${PYTHON_PATH}/py/objstrunicode.c
	${PYTHON_PATH}/py/objstringio.c
	${PYTHON_PATH}/py/objtuple.c
	${PYTHON_PATH}/py/objtype.c
	${PYTHON_PATH}/py/objzip.c
	${PYTHON_PATH}/py/opmethods.c
	${PYTHON_PATH}/py/sequence.c
	${PYTHON_PATH}/py/stream.c
	${PYTHON_PATH}/py/binary.c
	${PYTHON_PATH}/py/builtinimport.c
	${PYTHON_PATH}/py/builtinevex.c
	${PYTHON_PATH}/py/builtinhelp.c
	${PYTHON_PATH}/py/modarray.c
	${PYTHON_PATH}/py/modbuiltins.c
	${PYTHON_PATH}/py/modcollections.c
	${PYTHON_PATH}/py/modgc.c
	${PYTHON_PATH}/py/modio.c
	${PYTHON_PATH}/py/modmath.c
	${PYTHON_PATH}/py/modcmath.c
	${PYTHON_PATH}/py/modmicropython.c
	${PYTHON_PATH}/py/modstruct.c
	${PYTHON_PATH}/py/modsys.c
	${PYTHON_PATH}/py/moduerrno.c
	${PYTHON_PATH}/py/modthread.c
	${PYTHON_PATH}/py/vm.c
	${PYTHON_PATH}/py/bc.c
	${PYTHON_PATH}/py/showbc.c
	${PYTHON_PATH}/py/repl.c
	${PYTHON_PATH}/py/smallint.c
	${PYTHON_PATH}/py/frozenmod.c
        )

    set(PYTHON_EXTMOD_FILES
	${PYTHON_PATH}/extmod/moduasyncio.c
	${PYTHON_PATH}/extmod/moductypes.c
	${PYTHON_PATH}/extmod/modujson.c
	${PYTHON_PATH}/extmod/modure.c
	${PYTHON_PATH}/extmod/moduzlib.c
	${PYTHON_PATH}/extmod/moduheapq.c
	${PYTHON_PATH}/extmod/modutimeq.c
	${PYTHON_PATH}/extmod/moduhashlib.c
	${PYTHON_PATH}/extmod/moducryptolib.c
	${PYTHON_PATH}/extmod/modubinascii.c
	${PYTHON_PATH}/extmod/virtpin.c
	${PYTHON_PATH}/extmod/machine_mem.c
	${PYTHON_PATH}/extmod/machine_pinbase.c
	${PYTHON_PATH}/extmod/machine_signal.c
	${PYTHON_PATH}/extmod/machine_pulse.c
	${PYTHON_PATH}/extmod/machine_i2c.c
	${PYTHON_PATH}/extmod/machine_spi.c
	${PYTHON_PATH}/extmod/modbluetooth.c
	${PYTHON_PATH}/extmod/modussl_axtls.c
	${PYTHON_PATH}/extmod/modussl_mbedtls.c
	${PYTHON_PATH}/extmod/modurandom.c
	${PYTHON_PATH}/extmod/moduselect.c
	${PYTHON_PATH}/extmod/moduwebsocket.c
	${PYTHON_PATH}/extmod/modwebrepl.c
	${PYTHON_PATH}/extmod/modframebuf.c
	${PYTHON_PATH}/extmod/vfs.c
	${PYTHON_PATH}/extmod/vfs_blockdev.c
	${PYTHON_PATH}/extmod/vfs_reader.c
	${PYTHON_PATH}/extmod/vfs_posix.c
	${PYTHON_PATH}/extmod/vfs_posix_file.c
	${PYTHON_PATH}/extmod/vfs_fat.c
	${PYTHON_PATH}/extmod/vfs_fat_diskio.c
	${PYTHON_PATH}/extmod/vfs_fat_file.c
	${PYTHON_PATH}/extmod/vfs_lfs.c
	${PYTHON_PATH}/extmod/utime_mphal.c
	${PYTHON_PATH}/extmod/uos_dupterm.c
	${PYTHON_PATH}/lib/embed/abort_.c
	${PYTHON_PATH}/lib/utils/printf.c
        )

    set(PYTHON_PORT_FILES
	${PYTHON_PATH}/ports/unix/main.c
	${PYTHON_PATH}/ports/unix/gccollect.c
	${PYTHON_PATH}/ports/unix/unix_mphal.c
	${PYTHON_PATH}/ports/unix/mpthreadport.c
	${PYTHON_PATH}/ports/unix/input.c
	${PYTHON_PATH}/ports/unix/modmachine.c
	${PYTHON_PATH}/ports/unix/modos.c
	${PYTHON_PATH}/ports/unix/moduos_vfs.c
	${PYTHON_PATH}/ports/unix/modtime.c
	${PYTHON_PATH}/ports/unix/moduselect.c
	${PYTHON_PATH}/ports/unix/alloc.c
	${PYTHON_PATH}/ports/unix/fatfs_port.c
	${PYTHON_PATH}/ports/unix/mpbthciport.c
	${PYTHON_PATH}/ports/unix/mpbtstackport_common.c
	${PYTHON_PATH}/ports/unix/mpbtstackport_h4.c
	${PYTHON_PATH}/ports/unix/mpbtstackport_usb.c
	${PYTHON_PATH}/ports/unix/mpnimbleport.c
        )

    set(PYTHON_UTILS_FILES
        ${PYTHON_PATH}/lib/utils/gchelper_generic.c
        )

    set(APP_FILES
        ${PYTHON_CORE_FILES}
        ${PYTHON_PORT_FILES}
        ${PYTHON_EXTMOD_FILES}
        ${PYTHON_UTILS_FILES}
        )

    add_definitions(
        -DFFCONF_H="oofatfs/ffconf.h"
        -DMICROPY_PY_SYS_PLATFORM="cxx-kernel"
        -DUNDO_PRELUDE=0
        -DMICROPY_DEBUG_PARSE_RULE_NAME=1
        -DMICROPY_HACK_BACKSPACE=1
        #-DMICROPY_NLR_SETJMP
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

add_library(app
    ${APP_FILES}
    )

add_custom_command(TARGET app POST_BUILD
    COMMAND "${CROSS_COMPILE}objcopy" --redefine-sym main=app_main ${CMAKE_CURRENT_BINARY_DIR}/libapp.a
    )
