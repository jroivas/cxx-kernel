#!/bin/bash

set -eu

MICROPYTHON_VERSION="1.16"
wget -c "https://github.com/micropython/micropython/releases/download/v${MICROPYTHON_VERSION}/micropython-${MICROPYTHON_VERSION}.tar.xz"
tar xf "micropython-${MICROPYTHON_VERSION}.tar.xz"
cd "micropython-${MICROPYTHON_VERSION}"
# This is needed only if compiling kernel with -fomit-frame-pointer
#patch -p1 < ../micropython-${MICROPYTHON_VERSION}-prelude.patch
patch -p1 < ../micropython-${MICROPYTHON_VERSION}-backspace.patch
patch -p1 < ../micropython-${MICROPYTHON_VERSION}-musl-flush.patch
