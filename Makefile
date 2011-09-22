CXXFLAGS=-m32 -O2 -ffreestanding -nostdlib -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra
CFLAGS=-m32 -O2 -ffreestanding -nostdlib -fno-builtin -fno-exceptions -Wall -Werror -Wextra

all: kernel

run: kernel
	qemu -kernel kernel

run_img: floppy.img
	qemu -fda floppy.img -boot a

floppy.img: kernel pad
	cat grub-0.97-i386-pc/boot/grub/stage1 grub-0.97-i386-pc/boot/grub/stage2 pad kernel > floppy.img

pad:
	dd if=/dev/zero of=pad bs=1 count=750

#kernel: loader.o kernel.o video.o support.o cxa.o
kernel: loader.o kernel.o video.o main.o cxa.o
	ld -m elf_i386 -T link2.ld -o kernel $^
	#ld -m i386linux -T link2.ld -o kernel $^

#support.o: support.c
#	gcc -c $(CFLAGS) -o $@ $<

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

clean:
	rm -f kernel floppy.img *.o
