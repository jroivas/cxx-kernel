#!/bin/bash

script_dir=$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)

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
