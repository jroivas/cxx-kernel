#!/bin/bash

if [ -d "musl/.git" ] ; then
    cd musl && git pull --rebase
else
    git clone git://git.musl-libc.org/musl
fi
