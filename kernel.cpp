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

        Timer::getInstance()->setFrequency(1000);
}

Kernel::~Kernel()
{
	delete video;
	video = NULL;
}

int Kernel::run()
{

//	sti();
	if (video!=NULL) {
		video->clear();
		video->printf("Hello world!\n");
		video->printf("\nC++ kernel.\n");
		video->printf("And a TAB\t test!\n");
		video->printf("Removing letter A\bB and continuing.\n");
		video->printf("\b\b\bABCDEFG\n");
#if 0
		video->print((long)0);
		video->print("\n");
		video->print((long)1);
		video->print("\n");
		video->print(12345);
		video->print("\n");
		video->print(42);
		video->print("\n");
		video->print(-12345);
		video->print("\n");
		video->printU(1234567890);
		video->print("\n");
		video->printU(-1L);
		video->print("\n");
#endif
                video->printf("Heips %d kertaa %s ja %x ja %x!\n",(int)42,"kokopoo",(unsigned int)0x42,(unsigned int)0xab1);
		video->printf("RAI\n");
                //video->printf("Heips %x ja %x!\n",(unsigned int)0x42,(unsigned int)0xab1);

		//delete video;
	}

	//asm ("int $0x20");
	//asm ("int $0x1F");


	return 0;
}
