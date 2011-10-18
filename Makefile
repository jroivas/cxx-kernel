include config.mk
CXXFLAGS:=$(CXXFLAGS) -I.
CXXFLAGSO:=$(CXXFLAGSO) -I.
#OBJS=loader.o kernel.o video.o main.o cxa.o mutex.o local.o operators.o mm.o paging.o gdt.o string.o idt.o idt_handlers.o timer.o kb.o
OBJS=arch/$(ARCH)/loader.o kernel.o video.o main.o cxa.o mutex.o local.o operators.o mm.o paging.o gdt.o string.o idt.o timer.o kb.o fb.o math.o
THIRDPARTY=3rdparty/libx86emu.a
#THIRDPARTY=3rdparty/libx86.a
#THIRDPARTY=3rdparty/a/*.o

#LIBS=-Larch/ -larch
LIBS=arch/arch.a arch/$(ARCH)/$(ARCH).a

all: kernel

run: run_iso

run_qemu: kernel
	qemu -kernel kernel

kernel: link2.ld build_arch $(OBJS) $(LIBS) $(THIRDPARTY)
	$(LD) -m elf_i386 -T link2.ld -o kernel $(OBJS) $(LIBS) $(THIRDPARTY)

kernel.iso: kernel menu.lst stage2_eltorito
	mkdir -p isofiles/boot/grub
	cp -f kernel isofiles/boot/kernel
	cp -f stage2_eltorito isofiles/boot/grub/
	cp -f menu.lst isofiles/boot/grub/
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -quiet -boot-load-size 4 -boot-info-table -o kernel.iso ./isofiles

run_iso: kernel.iso
	qemu -cdrom kernel.iso

3rdparty/libx86emu.a:
	make -C 3rdparty

main.o: main.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

kernel.o: kernel.cpp kernel.h
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

paging.o: paging.cpp paging.h mmap.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

gdt.o: gdt.cpp gdt.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

string.o: string.cpp string.h memcopy.h
	$(CXX) -c $(CXXFLAGSO) -o $@ $< 

#port.o: port.cpp port.h
#	$(CXX) -c $(CXXFLAGS) -o $@ $< 

x86.o: x86.cpp x86.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

idt.o: idt.cpp idt.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

timer.o: timer.cpp timer.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

kb.o: kb.cpp kb.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

fb.o: fb.cpp fb.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

states.o: states.cpp states.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

math.o: math.cpp math.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

arch/$(ARCH)/loader.o:
arch/$(ARCH)/$(ARCH).a:
arch/arch.a:
build_arch:
	make -C arch

clean:
	make -C arch clean
	make -C 3rdparty clean
	rm -f kernel kernel.iso *.o
