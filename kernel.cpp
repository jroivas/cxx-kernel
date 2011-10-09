#include "types.h"
#include "kernel.h"
#include "idt.h"
#include "timer.h"
#include "kb.h"

Kernel::Kernel()
{
	//video = new Video();
	video = Video::get();
	platform = new Platform();

	IDT::get()->initISR();
	IDT::get()->initIRQ();

        Timer::get()->setFrequency(KERNEL_FREQUENCY);
        KB::get();
}

Kernel::~Kernel()
{
	delete video;
	video = NULL;
}

int Kernel::run()
{

	platform->state()->startInterrupts();
	if (video!=NULL) {
		video->clear();
		video->printf("Ticks: %lu!\n",Timer::get()->getTicks());
		video->printf("Hello world!\n");
		video->printf("\nC++ kernel.\n");
		video->printf("And a TAB\t test!\n");
		video->printf("Removing letter A\bB and continuing.\n");
		video->printf("\b\b\bABCDEFG\n");
		video->printf("Ticks: %lu!\n",Timer::get()->getTicks());

                /* Some random timing... */
                volatile int b = 5;
                for (int i=0; i<0x4FFFFFF; i++) { b = b + i; }
		video->printf("Ticks: %lu!\n",Timer::get()->getTicks());
                Timer::get()->wait(100);
		video->printf("Ticks: %lu!\n",Timer::get()->getTicks());

		//delete video;
	}
	while(1) {}

	//asm ("int $0x20");
	//asm ("int $0x1F");


	return 0;
}
