#!/bin/bash

MICROPYTHON_VERSION="1.16"
wget -c "https://github.com/micropython/micropython/releases/download/v${MICROPYTHON_VERSION}/micropython-${MICROPYTHON_VERSION}.tar.xz"
tar xf "micropython-${MICROPYTHON_VERSION}.tar.xz"
