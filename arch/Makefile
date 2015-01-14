include ../config.mk
CXXFLAGS:=$(CXXFLAGS) -I../ -I. -I../inc
CXXFLAGSO:=$(CXXFLAGSO) -I../ -I. -I../inc


#all: platform.o compile_arch
all: compile_arch platform.o

platform.o: platform.cpp

$(ARCH)/$(ARCH).a:
compile_arch:
	make -C $(ARCH)

clean:
	rm -rf *.o *.a
	make -C x86 clean
	make -C arm clean
	make -C linux clean
