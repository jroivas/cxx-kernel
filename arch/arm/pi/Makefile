include ../../../config.mk
CXXFLAGS:=$(CXXFLAGS) -I../../../ -I../../ -I..

OBJS=armfb.o

all: pi

pi: $(OBJS)
	mv -f $(OBJS) ../

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

clean:
	rm -rf *.o *.a
