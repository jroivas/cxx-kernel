# General build configuration

CXX=clang++
CC=clang
TUNE=-m32 -mtune=pentium-mmx -mno-sse -mmmx
#TUNE=-m32 -mno-sse -mmmx
#CC=gcc
#CXX=g++

CFLAGS=-DNO_SYS_HEADERS -D__FREESTAND__ $(TUNE) -ffreestanding -fno-builtin -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions -nostdinc -nostdlib -fno-stack-protector
CXXFLAGS=-DNO_SYS_HEADERS -D__FREESTAND__ $(TUNE) -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions

#x86: CXXFLAGS:=-m32

#CFLAGS=-DNO_SYS_HEADERS -D__FREESTAND__ -m32 $(TUNE) -ffreestanding -fno-builtin -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions -nostdinc -nostdlib -fno-stack-protector
#CXXFLAGS=-DNO_SYS_HEADERS -D__FREESTAND__ -m32 $(TUNE) -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions

#CXXFLAGS_CC=-D__FREESTAND__ -m32 -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -fomit-frame-pointer -finline-functions
CXXFLAGSO=$(CXXFLAGS)
#CXXFLAGSO=-O0 $(CXXFLAGS)
#CXXFLAGSO=$(CXXFLAGS)
LD=ld

#ARCH=x86
