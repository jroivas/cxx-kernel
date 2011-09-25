CXX=clang++
#CXX=g++
#CXXFLAGS=-m32 -ffreestanding -nostdlib -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc
CXXFLAGS=-m32 -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions -nostdinc
OBJS=loader.o kernel.o video.o main.o cxa.o mutex.o local.o operators.o mm.o paging.o gdt.o string.o port.o

all: kernel

run: kernel
	qemu -kernel kernel

kernel: link2.ld $(OBJS)
	ld -m elf_i386 -T link2.ld -o kernel $(OBJS)

run_img: floppy.img
	qemu -fda floppy.img -boot a

floppy.img: kernel pad
	cat grub/boot/grub/stage1 grub/boot/grub/stage2 pad kernel > floppy.img

pad:
	dd if=/dev/zero of=pad bs=1 count=750

loader.o: loader.asm
	nasm -f elf32 -o $@ $<

main.o: main.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

kernel.o: kernel.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

cxa.o: cxa.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

video.o: video.cpp video.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

mutex.o: mutex.cpp mutex.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

local.o: local.cpp local.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

operators.o: operators.cpp operators.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

mm.o: mm.cpp mm.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

paging.o: paging.cpp paging.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

gdt.o: gdt.cpp gdt.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

string.o: string.cpp string.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

port.o: port.cpp port.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

clean:
	rm -f kernel floppy.img *.o
