include ../../config.mk
CXXFLAGS:=$(CXXFLAGS) -I../../ -I../../3rdparty/my_x86emu/

OBJS=loader.o paginglinux.o

all: $(OBJS)
	mv loader.o ../

loader.o: loader.cpp

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

clean:
	rm -rf *.o *.a
