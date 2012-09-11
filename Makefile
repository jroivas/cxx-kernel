
all: 
	@echo "*** Please select your target:"
	@echo "make arm     -  QEMU ARM"
	@echo "make arm-pi  -  Raspberry Pi ARM"
	@echo "make linux   -  Native Linux binary"
	@echo "make x86     -  X86 bootable"
	@echo "*****************************"

x86:
	make ARCH=x86 -f Makefile.arches

arm:
	make ARCH=arm PLATFORM=qemu -f Makefile.arches

arm-pi:
	make ARCH=arm PLATFORM=pi -f Makefile.arches

linux:
	make ARCH=linux -f Makefile.arches

invalid:
	make MARCH=invalid -f Makefile.arches

clean:
	make ARCH=all -f Makefile.arches clean

run_x86:
	make ARCH=x86 -f Makefile.arches run_x86

run_arm:
	make ARCH=arm PLATFORM=qemu -f Makefile.arches run_arm
