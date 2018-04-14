#!/bin/bash

script_dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)

update_src() {
    if [ -d "${script_dir}/musl/.git" ] ; then
        cd "${script_dir}/musl" && git pull
    else
        cd "${script_dir}"
        git clone git://git.musl-libc.org/musl
        cd "${script_dir}/musl"
        find "${script_dir}/patches" -name "*.patch" | sort | while read patch
        do
            patch -p1 < "$patch"
        done
    fi
}

gen_syscall() {
    find "${script_dir}/musl/arch" -name syscall.h.in | while read src; do
        dir=$(dirname ${src})
        out=$(basename ${src} .in)
        #sed -n -e "s/__NR_/SYS_/g" < "${src}" > "${dir}/${out}"
        sed "s/__NR_/SYS_/g" < "${src}" > "${dir}/${out}"
    done
}

gen_alltypes() {
    find "${script_dir}/musl/arch" -name alltypes.h.in | while read src; do
        dir=$(dirname ${src})
        out=$(basename ${src} .in)
        sed -f "${script_dir}/musl/tools/mkalltypes.sed" "${src}" "${script_dir}/musl/include/alltypes.h.in" > "${dir}/${out}"
        sed -i "s/typedef unsigned _Addr size_t/typedef long unsigned _Addr size_t/" "${dir}/${out}"
    done
}

gen_version() {
    echo "#define VERSION \"$(<"${script_dir}/musl/VERSION")\"" > ${script_dir}/musl/src/internal/version.h
}


update_src
gen_syscall
gen_alltypes
gen_version
