#include "types.h"
#include "kernel.h"
#include "idt.h"
#include "x86.h"
#include "timer.h"

Kernel::Kernel()
{
	video = new Video();

	IDT::getInstance()->initISR();
	IDT::getInstance()->initIRQ();

        Timer::getInstance()->setFrequency(KERNEL_FREQUENCY);
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
		video->printf("Ticks: %lu!\n",Timer::getInstance()->getTicks());
		video->printf("Hello world!\n");
		video->printf("\nC++ kernel.\n");
		video->printf("And a TAB\t test!\n");
		video->printf("Removing letter A\bB and continuing.\n");
		video->printf("\b\b\bABCDEFG\n");
		video->printf("Ticks: %lu!\n",Timer::getInstance()->getTicks());

                /* Some random timing... */
                volatile int b = 5;
                for (int i=0; i<0x4FFFFFF; i++) { b = b + i; }
		video->printf("Ticks: %lu!\n",Timer::getInstance()->getTicks());
                Timer::getInstance()->wait(100);
		video->printf("Ticks: %lu!\n",Timer::getInstance()->getTicks());

		//delete video;
	}

	//asm ("int $0x20");
	//asm ("int $0x1F");


	return 0;
}
