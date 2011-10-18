include ../config.mk
CXXFLAGS:=$(CXXFLAGS) -I../
CXXFLAGSO:=$(CXXFLAGSO) -I../

OBJS=platform.o $(ARCH)/$(ARCH).a
LIBS=$(ARCH)/$(ARCH).a

all: arch.a

arch.a: $(OBJS) compile_arch
	ar rcs $@ $(OBJS)
	ranlib $@

platform.o: platform.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

$(ARCH)/$(ARCH).a:
compile_arch:
	make -C $(ARCH)

clean:
	rm -rf *.o *.a
	make -C x86 clean
