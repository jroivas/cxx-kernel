# General build configuration

CXX=clang++
CC=clang
#CXX=g++
CFLAGS=-m32 -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions
CXXFLAGS=-m32 -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions
CXXFLAGSO=-O2 $(CXXFLAGS)
LD=ld

ARCH=x86
