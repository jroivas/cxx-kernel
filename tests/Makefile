CXXFLAGS=-I . -I..

all: test_all

build_all: test_list

test_all: build_all
	@./test_list

test_list: test_list.o ../list.o
	$(CXX) -o $@ $^

test_list.o: test_list.cpp

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

clean:
	rm -rf *.o test_list
	make -C .. clean
