# General build configuration

CXX=clang++
CC=clang
#CC=gcc
#CXX=g++
#CFLAGS=-D__FREESTAND__ -DNO_LONG_LONG -m32 -ffreestanding -fno-builtin -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions -nostdlib
#CFLAGS=-D__FREESTAND__ -DNO_LONG_LONG -m32 -ffreestanding -fno-builtin -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions -nostdinc -nostdlib
CFLAGS=-DNO_SYS_HEADERS -D__FREESTAND__ -m32 -ffreestanding -fno-builtin -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions -nostdinc -nostdlib -fno-stack-protector
CXXFLAGS=-DNO_SYS_HEADERS -D__FREESTAND__ -m32 -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions
#CXXFLAGS_CC=-D__FREESTAND__ -m32 -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -fomit-frame-pointer -finline-functions
CXXFLAGSO=$(CXXFLAGS)
#CXXFLAGSO=-O0 $(CXXFLAGS)
#CXXFLAGSO=$(CXXFLAGS)
LD=ld

ARCH=x86
