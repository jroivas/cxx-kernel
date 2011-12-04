#include "types.h"
#include "kernel.h"
#include "idt.h"
#include "timer.h"
#include "kb.h"

Kernel::Kernel()
{
	//video = new Video();
	platform = new Platform();
	video = Video::get();

	if (IDT::get()==NULL) {
		unsigned short *tmp = (unsigned short *)(0xB8006);
		*tmp = 0x9745; //E
		while(1) ;
	}
	IDT::get()->initISR();
	//while(1);
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
	//platform->fb();
	//platform->fb()->query(NULL);
	while(1) {}

	//asm ("int $0x20");
	//asm ("int $0x1F");


	return 0;
}
