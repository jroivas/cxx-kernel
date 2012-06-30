
all: x86 arm linux

x86:
	make ARCH=x86 -f Makefile.arches

arm:
	make ARCH=arm -f Makefile.arches

linux:
	make ARCH=linux -f Makefile.arches

invalid:
	make MARCH=invalid -f Makefile.arches

clean:
	make ARCH=all -f Makefile.arches clean
