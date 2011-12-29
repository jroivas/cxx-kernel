all: x86emu bootfont

bootfont:
	make -C font

x86emu:
	make -C my_x86emu clean
	make -C my_x86emu

clean:
	make -C my_x86emu clean
	make -C font clean
