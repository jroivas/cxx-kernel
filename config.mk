# General build configuration

CXX=clang++
CC=clang
#CC=gcc
#CXX=g++
#CFLAGS=-D__FREESTAND__ -DNO_LONG_LONG -m32 -ffreestanding -fno-builtin -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions -nostdlib
#CFLAGS=-D__FREESTAND__ -DNO_LONG_LONG -m32 -ffreestanding -fno-builtin -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions -nostdinc -nostdlib
CFLAGS=-D__FREESTAND__ -m32 -ffreestanding -fno-builtin -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions -nostdinc -nostdlib
CXXFLAGS=-m32 -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions
CXXFLAGSO=-O2 $(CXXFLAGS)
#CXXFLAGSO=$(CXXFLAGS)
LD=ld

ARCH=x86
