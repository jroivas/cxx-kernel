include ../../config.mk
CXXFLAGS:=$(CXXFLAGS) -I../../

OBJS=statesx86.o x86.o

all: x86.a

x86.a: $(OBJS)
	ar rcs $@ $(OBJS)
	ranlib $@

statesx86.o: statesx86.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

x86.o: x86.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

clean:
	rm -rf *.o *.a
