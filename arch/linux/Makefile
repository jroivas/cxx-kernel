include ../../config.mk
CXXFLAGS:=$(CXXFLAGS) -I../../ -I../../3rdparty/my_x86emu/

OBJS=loader.o dummy.o

all: $(OBJS)
	mv loader.o ../

loader.o: loader.cpp

dummy.o: dummy.cpp

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

clean:
	rm -rf *.o *.a
