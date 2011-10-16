
all: fetch libx86.a libx86_64.a libx86_32.a

fetch:
	./fetch.sh

libx86.a: clean
	make -C libx86 clean
	cd libx86 && CFLAGS="-m32" make static BACKEND=""
	cp libx86/libx86.a .

libx86_32.a: clean
	make -C libx86 clean
	cd libx86 && CFLAGS="-m32" make static BACKEND="x86emu"
	cp libx86/libx86.a libx86_32.a

libx86_64.a: clean
	make -C libx86 clean
	cd libx86 &&  CFLAGS="-m64" make static BACKEND="x86emu"
	cp libx86/libx86.a libx86_64.a

clean:
	make -C libx86 clean
	rm -f libx86*.a
