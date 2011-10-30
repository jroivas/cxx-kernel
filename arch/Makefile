include ../config.mk
CXXFLAGS:=$(CXXFLAGS) -I../
CXXFLAGSO:=$(CXXFLAGSO) -I../

#OBJS=platform.o $(ARCH)/$(ARCH).a
#LIBS=$(ARCH)/$(ARCH).a

#all: arch.a
#	$(CXX) -c $(CXXFLAGS) -o $@ $< 

#arch.a: $(OBJS) compile_arch
#	ar rcs $@ $(OBJS)
#	ranlib $@
all: platform.o compile_arch

platform.o: platform.cpp

$(ARCH)/$(ARCH).a:
compile_arch:
	make -C $(ARCH)

clean:
	rm -rf *.o *.a
	make -C $(ARCH) clean
