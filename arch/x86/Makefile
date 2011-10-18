include ../../config.mk
#CXXFLAGS:=$(CXXFLAGS) -I../../ -I../../3rdparty/libx86/x86emu/x86emu/ -I../../3rdparty/libx86/ -I../../3rdparty/libx86/x86emu/include
CXXFLAGS:=$(CXXFLAGS) -I../../ -I../../3rdparty/x86emu/

#OBJS=loader.o idt_handlers.o statesx86.o x86.o port.o timerx86.o videox86.o idtx86.o kbx86.o
OBJS=idt_handlers.o statesx86.o x86.o port.o timerx86.o videox86.o idtx86.o kbx86.o vesa.o bios.o
#OBJS=statesx86.o x86.o port.o timerx86.o videox86.o idtx86.o kbx86.o vesa.o

all: x86.a loader.o

x86.a: $(OBJS)
	ar rcs $@ $(OBJS)
	ranlib $@

loader.o: loader.asm
	nasm -f elf32 -o $@ $<

idt_handlers.o: idt_handlers.asm
	nasm -f elf32 -o $@ $<

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
