include ../../config.mk
CXXFLAGS:=$(CXXFLAGS) -I../../

OBJS=loader.o paginglinux.o idtlinux.o timerlinux.o stateslinux.o videolinux.o x11.o virtualdisc.o

all: $(OBJS)
	mv loader.o ../

loader.o: loader.cpp

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

clean:
	rm -rf *.o *.a
