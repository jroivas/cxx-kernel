include config.mk
CXXFLAGS:=$(CXXFLAGS) -I.
CXXFLAGSO:=-O2 $(CXXFLAGSO) -I.
#OBJS=loader.o kernel.o video.o main.o cxa.o mutex.o local.o operators.o mm.o paging.o gdt.o string.o idt.o idt_handlers.o timer.o kb.o
#OBJS=arch/$(ARCH)/loader.o kernel.o video.o main.o cxa.o mutex.o local.o operators.o mm.o paging.o gdt.o string.o idt.o timer.o kb.o fb.o math.o states.o
OBJS=arch/loader.o kernel.o video.o main.o cxa.o mutex.o local.o operators.o mm.o paging.o gdt.o string.o idt.o timer.o kb.o fb.o math.o states.o setjmp.o bits.o
#OBJS=kernel.o video.o main.o cxa.o mutex.o local.o operators.o mm.o paging.o gdt.o string.o idt.o timer.o kb.o fb.o math.o states.o
#THIRDPARTY=3rdparty/libx86emu.a
#THIRDPARTY=3rdparty/libx86.a
#THIRDPARTY=3rdparty/a/*.o
#THIRDPARTY=3rdparty/fb/x86emu.o
THIRDPARTY=3rdparty/my_x86emu/x86emu.o

#LIBS=-Larch/ -larch
LIBS=arch/arch.a arch/$(ARCH)/$(ARCH).a

all: kernel

run: run_iso

run_qemu: kernel
	qemu -kernel kernel

kernel: link2.ld build_arch $(OBJS) $(LIBS) 3rdparty/libx86emu.a
	#$(LD) -m elf_i386 -nostdlib -T link2.ld -o kernel $(OBJS) $(LIBS) $(THIRDPARTY)
	#$(LD) -m elf_i386 -nostdlib -T link2.ld -o kernel $(OBJS) arch/platform.o arch/$(ARCH)/*.o $(THIRDPARTY)
	$(LD) -m elf_i386 -nostdlib -T link2.ld -o kernel $(OBJS) arch/platform.o arch/$(ARCH)/*.o $(THIRDPARTY)
	#$(LD) -m elf_i386 -nostdlib -T link2.ld -o kernel $(OBJS) arch/platform.o arch/$(ARCH)/*.o
	#$(LD) -m elf_i386 -nostdlib -T link2.ld -o kernel $(OBJS) $(LIBS) $(THIRDPARTY)
	#$(CXX) -m32 -Xlinker -T -Xlinker link2.ld -ffreestanding -fno-builtin -nostdlib -s  -o  kernel $(OBJS) arch/platform.o arch/$(ARCH)/*.o $(THIRDPARTY)

kernel.iso: kernel menu.lst stage2_eltorito
	mkdir -p isofiles/boot/grub
	cp -f kernel isofiles/boot/kernel
	cp -f stage2_eltorito isofiles/boot/grub/
	cp -f menu.lst isofiles/boot/grub/
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -quiet -boot-load-size 4 -boot-info-table -o kernel.iso ./isofiles

run_iso: kernel.iso
	#qemu -no-kvm -cdrom kernel.iso
	qemu -cdrom kernel.iso

3rdparty/libx86emu.a:
	make -C 3rdparty

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

main.o: main.cpp

kernel.o: kernel.cpp kernel.h

cxa.o: cxa.cpp

video.o: video.cpp video.h
	$(CXX) -c $(CXXFLAGSO) -o $@ $< 

mutex.o: mutex.cpp mutex.h
	$(CXX) -c $(CXXFLAGSO) -o $@ $< 

local.o: local.cpp local.h

operators.o: operators.cpp operators.h
	$(CXX) -c $(CXXFLAGSO) -o $@ $< 

mm.o: mm.cpp mm.h
	$(CXX) -c $(CXXFLAGSO) -o $@ $< 

paging.o: paging.cpp paging.h mmap.h

gdt.o: gdt.cpp gdt.h

string.o: string.cpp string.h memcopy.h
	$(CXX) -c $(CXXFLAGSO) -o $@ $< 

x86.o: x86.cpp x86.h

idt.o: idt.cpp idt.h

timer.o: timer.cpp timer.h

kb.o: kb.cpp kb.h

fb.o: fb.cpp fb.h

states.o: states.cpp states.h

math.o: math.cpp math.h

setjmp.o: setjmp.cpp setjmp.h

arch/$(ARCH)/loader.o:
arch/$(ARCH)/$(ARCH).a:
arch/arch.a:
build_arch:
	make -C arch

clean:
	make -C arch clean
	make -C 3rdparty clean
	rm -f kernel kernel.iso *.o *.a
