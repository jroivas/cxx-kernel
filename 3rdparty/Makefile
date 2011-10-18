#OBJS=libx86.a libx86_64.a libx86_32.a
#OBJS=libx86_64.a libx86_32.a
#OBJS=libx86_32.a
OBJS=libx86.a
#libx86emu.a

all: libx86emu.a

fetch:
	./fetch.sh

libx86emu.a:
	make -C x86emu
	cp x86emu/libx86emu.a .

#libx86.a: clean
#	make -C libx86 clean
#	cd libx86 && CFLAGS="-m32" make static BACKEND=""
#	cp libx86/libx86.a .
#	ranlib libx86.a

libx86.a: clean
	make -C libx86 clean
	cd libx86 && make static BACKEND="x86emu"
	cp libx86/libx86.a libx86.a
	ranlib libx86.a
	cd a && ar x ../libx86.a

libx86_32.a: clean
	make -C libx86 clean
	cd libx86 && CFLAGS="-m32" make static BACKEND="x86emu"
	cp libx86/libx86.a libx86_32.a
	ranlib libx86_32.a
	cd a && ar x ../libx86_32.a

libx86_64.a: clean
	make -C libx86 clean
	cd libx86 &&  CFLAGS="-m64" make static BACKEND="x86emu"
	cp libx86/libx86.a libx86_64.a
	ranlib libx86_64.a

clean:
	make -C x86emu clean
	#make -C libx86 clean
	#rm -f libx86*.a
	rm -f libx86emu.a
