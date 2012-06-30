#!/bin/bash
if [ ! -d "isofiles/boot/kernel" ] ; then
	mkdir -p isofiles/boot/kernel
fi
if [ -e "stage2_eltorito" ] ; then
	cp -f stage2_eltorito isofiles/boot/grub/
fi
if [ -e "menu.lst" ] ; then
	cp -f menu.lst isofiles/boot/grub/
fi
if [ -e "cxx-test-kernel" ] ; then
	cp -f cxx-test-kernel isofiles/boot/kernel
fi

$(which genisoimage) -R -b boot/grub/stage2_eltorito -no-emul-boot -quiet -boot-load-size 4 -boot-info-table -o kernel.iso ./isofiles
