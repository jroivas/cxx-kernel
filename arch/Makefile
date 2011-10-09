include ../config.mk
CXXFLAGS:=$(CXXFLAGS) -I../
CXXFLAGSO:=$(CXXFLAGSO) -I../

OBJS=platform.o $(ARCH)/$(ARCH).a
LIBS=$(ARCH)/$(ARCH).a

all: arch.a

arch.a: $(OBJS)
	ar rcs $@ $(OBJS)
	ranlib $@

platform.o: platform.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

$(ARCH)/$(ARCH).a:
	make -C $(ARCH)

clean:
	rm -rf *.o *.a
	make -C x86 clean
