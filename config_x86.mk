# X86 build configuration

CROSS_COMPILE=
CXX?=$(CROSS_COMPILE)clang++-3.5
CC?=$(CROSS_COMPILE)clang-3.5
LD=$(CROSS_COMPILE)ld
#LD=$(CROSS_COMPILE)g++
AS=$(CROSS_COMPILE)as
TUNE=-m32 -mtune=pentium-mmx -mno-sse -mmmx
#TUNE=-m32 -mno-sse -mmmx
#CC=$(CROSS_COMPILE)gcc
#CXX=$(CROSS_COMPILE)g++

#CFLAGS=-DNO_SYS_HEADERS -D__FREESTAND__ $(TUNE) -ffreestanding -fno-builtin -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions -nostdinc -nostdlib -fno-stack-protector
CFLAGS=-DNO_SYS_HEADERS -D__FREESTAND__ $(TUNE) -ffreestanding -fno-builtin -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -nostdinc -nostdlib -fno-stack-protector
#CXXFLAGS=-DNO_SYS_HEADERS -D__FREESTAND__ $(TUNE) -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions
CXXFLAGS=-DNO_SYS_HEADERS -D__FREESTAND__ $(TUNE) -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer
CXXFLAGSO=$(CXXFLAGS)
