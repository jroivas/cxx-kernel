CXXFLAGS=-m32 -ffreestanding -nostdlib -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc
OBJS=loader.o kernel.o video.o main.o cxa.o mutex.o local.o operators.o mm.o paging.o gdt.o string.o port.o

all: kernel

run: kernel
	qemu -kernel kernel

run_img: floppy.img
	qemu -fda floppy.img -boot a

floppy.img: kernel pad
	cat grub-0.97-i386-pc/boot/grub/stage1 grub-0.97-i386-pc/boot/grub/stage2 pad kernel > floppy.img

pad:
	dd if=/dev/zero of=pad bs=1 count=750

kernel: link2.ld $(OBJS)
	ld -m elf_i386 -T link2.ld -o kernel $(OBJS)

loader.o: loader.asm
	#nasm -f aout -o $@ $<
	nasm -f elf -o $@ $<

main.o: main.cpp
	g++ -c $(CXXFLAGS) -o $@ $< 

kernel.o: kernel.cpp
	g++ -c $(CXXFLAGS) -o $@ $< 

cxa.o: cxa.cpp
	g++ -c $(CXXFLAGS) -o $@ $< 

video.o: video.cpp video.h
	g++ -c $(CXXFLAGS) -o $@ $< 

mutex.o: mutex.cpp mutex.h
	g++ -c $(CXXFLAGS) -o $@ $< 

local.o: local.cpp local.h
	g++ -c $(CXXFLAGS) -o $@ $< 

operators.o: operators.cpp operators.h
	g++ -c $(CXXFLAGS) -o $@ $< 

mm.o: mm.cpp mm.h
	g++ -c $(CXXFLAGS) -o $@ $< 

paging.o: paging.cpp paging.h
	g++ -c $(CXXFLAGS) -o $@ $< 

gdt.o: gdt.cpp gdt.h
	g++ -c $(CXXFLAGS) -o $@ $< 

string.o: string.cpp string.h
	g++ -c $(CXXFLAGS) -o $@ $< 

port.o: port.cpp port.h
	g++ -c $(CXXFLAGS) -o $@ $< 

clean:
	rm -f kernel floppy.img *.o
