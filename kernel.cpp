#include "types.h"
#include "kernel.h"
#include "idt.h"
#include "timer.h"
#include "kb.h"
#include "fb.h"
#include "ata.h"

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

//extern const struct gfb_font bold8x16;
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
                for (int i=0; i<0x1FFFFFF; i++) { b = b + i; }
		video->printf("Ticks: %lu!\n",Timer::get()->getTicks());
                Timer::get()->wait(100);
		video->printf("Ticks: %lu!\n",Timer::get()->getTicks());
                Timer::get()->msleep(50);
		video->printf("Ticks msleep(50): %lu!\n",Timer::get()->getTicks());

		//delete video;
	}
	PCI *p = Platform::pci();
	if (p!=NULL) {
		if (p->isAvailable()) video->printf("Found PCI\n");
		else video->printf("PCI not available\n");
		p->scanDevices();
	}
	ATA *a = new ATA();
	(void)a;
	for (int i=0; i<0x5FFFFFF; i++) 
		for (int j=0; j<0x22; j++) { }

	FB::ModeConfig conf;
	conf.width=800;
	conf.height=600;
	conf.depth=24;

	//FB::ModeConfig *vconf = platform->fb()->query(NULL);
	FB::ModeConfig *vconf = platform->fb()->query(&conf);
	if (vconf!=NULL) {
		platform->fb()->configure(vconf);
#if 1
		for (int j=0; j<120; j++) {
			platform->fb()->putPixel(j,10,255,255,255);
		}
		platform->fb()->swap();
		platform->fb()->blit();
#if 1
		for (int j=100; j<120; j++) {
			for (int i=100; i<200; i++) {
				platform->fb()->putPixel(i,100+j,255,0,0);
				platform->fb()->putPixel(j,100+i,0,0,255);
			}
		}
		platform->fb()->swap();
		platform->fb()->blit();
#endif
#endif
#if 1
		video->printf("C++ test kernel\n");
		video->printf("Note to self: %x\n",0x1234);
		platform->fb()->swap();
		platform->fb()->blit();
#endif
	}
	while(1) {}

	return 0;
}
