CXX=clang++
#CXX=g++
#CXXFLAGS=-m32 -ffreestanding -nostdlib -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc
#CXXFLAGS=-m32 -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions -nostdinc
#CXXFLAGS=-m32 -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions
#CXXFLAGS=-m32 -ffreestanding -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions
#OF=-fguess-branch-probability -fif-conversion2 -fif-conversion -fmerge-constants -fsplit-wide-types -ftree-builtin-call-dce -ftree-ccp -ftree-ch -ftree-copyrename -ftree-dce -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre -ftree-phiprop -ftree-sra -ftree-pta -ftree-ter -funit-at-a-time
CXXFLAGS=-m32 -ffreestanding -fno-builtin -fno-rtti -fno-exceptions -Wall -Werror -Wextra -fomit-frame-pointer -finline-functions
CXXFLAGSO=-O2 $(CXXFLAGS)
OBJS=loader.o kernel.o video.o main.o cxa.o mutex.o local.o operators.o mm.o paging.o gdt.o string.o port.o x86.o idt.o idt_handlers.o timer.o

all: kernel

run: run_iso

run_qemu: kernel
	qemu -kernel kernel

kernel: link2.ld $(OBJS)
	ld -m elf_i386 -T link2.ld -o kernel $(OBJS)

kernel.iso: kernel menu.lst stage2_eltorito
	mkdir -p isofiles/boot/grub
	cp -f kernel isofiles/boot/kernel
	cp -f stage2_eltorito isofiles/boot/grub/
	cp -f menu.lst isofiles/boot/grub/
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -quiet -boot-load-size 4 -boot-info-table -o kernel.iso ./isofiles

run_iso: kernel.iso
	qemu -cdrom kernel.iso

loader.o: loader.asm
	nasm -f elf32 -o $@ $<

idt_handlers.o: idt_handlers.asm
	nasm -f elf32 -o $@ $<

main.o: main.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

kernel.o: kernel.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

cxa.o: cxa.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

video.o: video.cpp video.h
	$(CXX) -c $(CXXFLAGSO) -o $@ $< 

mutex.o: mutex.cpp mutex.h
	$(CXX) -c $(CXXFLAGSO) -o $@ $< 

local.o: local.cpp local.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

operators.o: operators.cpp operators.h
	$(CXX) -c $(CXXFLAGSO) -o $@ $< 

mm.o: mm.cpp mm.h
	$(CXX) -c $(CXXFLAGSO) -o $@ $< 

paging.o: paging.cpp paging.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

gdt.o: gdt.cpp gdt.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

string.o: string.cpp string.h
	$(CXX) -c $(CXXFLAGSO) -o $@ $< 

port.o: port.cpp port.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

x86.o: x86.cpp x86.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

idt.o: idt.cpp idt.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

timer.o: timer.cpp timer.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

clean:
	rm -f kernel kernel.iso *.o
