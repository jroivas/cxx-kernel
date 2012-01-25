include config.mk
CXXFLAGS:=$(CXXFLAGS) -I. -Iarch
CXXFLAGSO:=-O2 $(CXXFLAGSO) -I. -Iarch
OBJS=arch/loader.o kernel.o video.o main.o cxa.o mutex.o local.o operators.o mm.o paging.o gdt.o string.o idt.o timer.o kb.o fb.o math.o states.o setjmp.o bits.o font.o pci.o ata.o storage.o task.o list.o processmanager.o
THIRDPARTY=3rdparty/my_x86emu/x86emu.o 3rdparty/font/boot_font.o

#LIBS=arch/arch.a
LIBS=

.PHONY: all clean x86 linux

all: x86

config_h_pre:
	echo '#ifndef CONFIG_H' > config.h
	echo '#define CONFIG_H' >> config.h

config_h_post:
	echo '#endif' >> config.h

config_h_x86:
	echo '#define ARCH_x86' >> config.h

config_h_linux:
	echo '#define ARCH_LINUX' >> config.h

x86: config_h_pre config_h_x86 config_h_post kernel_x86 kernel.iso

linux: config_h_pre config_h_linux config_h_post kernel_linux

run: run_iso

run_qemu: kernel
	qemu -kernel kernel

kernel_linux: link2.ld arch_linux $(OBJS) $(LIBS) 3rdparty_libs
	$(CXX) -o kernel_linux $(OBJS) arch/platform.o arch/linux/*.o $(THIRDPARTY) -lc -lpthread -lX11

kernel_x86: link2.ld arch_x86 $(OBJS) arch/x86/x86.a $(LIBS) 3rdparty_libs
	$(LD) -m elf_i386 -nostdlib -T link2.ld -o kernel $(OBJS) arch/platform.o arch/x86/*.o $(THIRDPARTY)
	#$(CXX) -m32 -Xlinker -T -Xlinker link2.ld -ffreestanding -fno-builtin -nostdlib -s  -o  kernel $(OBJS) arch/platform.o arch/$(ARCH)/*.o $(THIRDPARTY)

kernel.iso: kernel menu.lst stage2_eltorito
	mkdir -p isofiles/boot/grub
	cp -f kernel isofiles/boot/kernel
	cp -f stage2_eltorito isofiles/boot/grub/
	cp -f menu.lst isofiles/boot/grub/
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -quiet -boot-load-size 4 -boot-info-table -o kernel.iso ./isofiles

run_iso: all kernel.iso test.img
	#qemu -serial mon:stdio -no-kvm -cdrom kernel.iso
	qemu -serial mon:stdio -cdrom kernel.iso -hda test.img

test.img:
	qemu-img info test.img || qemu-img create -f raw test.img 256M

3rdparty_libs:
	make -C 3rdparty

3rdparty/libx86emu.a:
	make -C 3rdparty

.cpp.o:
	$(CXX) -c $(CXXFLAGSO) -o $@ $< 

.c.o:
	$(CC) -c $(CXXFLAGSO) -o $@ $< 

gdt.o: gdt.cpp gdt.h

idt.o: idt.cpp idt.h

timer.o: timer.cpp timer.h
	$(CXX) -c $(CXXFLAGS) -o $@ $< 


arch_x86:
	make ARCH=x86 -C arch

arch_linux:
	make ARCH=linux -C arch

clean:
	rm -f config.h
	make -C arch clean
	make -C 3rdparty clean
	rm -f kernel kernel.iso *.o *.a
	rm -f 3drparty/font/*.o 
