include ../config.mk
CXXFLAGS:=$(CXXFLAGS) -I../
CXXFLAGSO:=$(CXXFLAGSO) -I../


all: platform.o compile_arch

platform.o: platform.cpp

$(ARCH)/$(ARCH).a:
compile_arch:
	make -C $(ARCH)

clean:
	rm -rf *.o *.a
	make -C x86
	make -C linux
