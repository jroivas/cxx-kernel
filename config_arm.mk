# ARM build configuration

CROSS_COMPILE=~/bin/arm-2010q1/bin/arm-none-eabi-
LD=$(CROSS_COMPILE)ld
AS=$(CROSS_COMPILE)as
TUNE=
CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++

CFLAGS=-DNO_SYS_HEADERS -D__FREESTAND__ $(TUNE) -ffreestanding -fno-builtin -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions -nostdinc -nostdlib -fno-stack-protector
CXXFLAGS=-DNO_SYS_HEADERS -D__FREESTAND__ $(TUNE) -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions
CXXFLAGSO=$(CXXFLAGS)
