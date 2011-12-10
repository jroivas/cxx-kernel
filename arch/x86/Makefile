include ../../config.mk
CXXFLAGS:=$(CXXFLAGS) -I../../ -I../../3rdparty/my_x86emu/

OBJS=idt_handlers.o statesx86.o x86.o port.o timerx86.o videox86.o idtx86.o kbx86.o vesa.o bios.o setjmp.o pagingx86.o pages.o memcpy.o gdt.o

all: x86.a loader.o

x86.a: $(OBJS)
	ar rcs $@ $(OBJS)
	ranlib $@

loader.o: loader.asm
	nasm -f elf32 -o $@ $<
	mv loader.o ../

idt_handlers.o: idt_handlers.asm
	nasm -f elf32 -o $@ $<

setjmp.o: setjmp.asm
	nasm -f elf32 -o $@ $<

memcpy.o: memcpy.asm
	nasm -f elf32 -o $@ $<

pages.o: pages.asm
	nasm -f elf32 -o $@ $<

gdt.o: gdt.asm
	nasm -f elf32 -o $@ $<

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

statesx86.o: statesx86.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

timerx86.o: timerx86.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

videox86.o: videox86.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

x86.o: x86.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

idtx86.o: idtx86.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

kbx86.o: kbx86.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

port.o: port.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

vesa.o: vesa.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

bios.o: bios.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

clean:
	rm -rf *.o *.a
