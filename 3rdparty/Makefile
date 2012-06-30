TARGETS=bootfont
ifeq (x86,$(ARCH))
TARGETS+=x86emu
else
endif

all: $(TARGETS)

bootfont:
	make -C font

x86emu:
ifeq (x86,$(ARCH))
	make -C my_x86emu clean
	make -C my_x86emu
endif

clean:
	make -C my_x86emu clean
	make -C font clean
