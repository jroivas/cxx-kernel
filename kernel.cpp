#include "types.h"
#include "kernel.h"
#include "idt.h"
#include "x86.h"

Kernel::Kernel()
{
	video = new Video();
	IDT::getInstance()->initISR();
	IDT::getInstance()->initIRQ();
}

Kernel::~Kernel()
{
	delete video;
	video = NULL;
}

int Kernel::run()
{

	sti();
	if (video!=NULL) {
		video->clear();
		video->print("Hello world!\n");
		video->print("\nC++ kernel.\n");
		video->print("And a TAB\t test!\n");
		video->print("Removing letter A\bB and continuing.\n");
		video->print("\b\b\bABCDEFG\n");

		delete video;
	}

	asm ("int $0x20");
	//asm ("int $0x1F");


	return 0;
}
