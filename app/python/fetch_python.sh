#!/bin/bash

PYTHON_VERSION="3.4.2"
wget -c "https://www.python.org/ftp/python/${PYTHON_VERSION}/Python-${PYTHON_VERSION}.tar.xz"
tar xf "Python-${PYTHON_VERSION}.tar.xz"
cd "Python-${PYTHON_VERSION}" && patch -p1 < ../python-${PYTHON_VERSION}.patch
