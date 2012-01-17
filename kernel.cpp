#include "types.h"
#include "kernel.h"
#include "idt.h"
#include "timer.h"
#include "kb.h"
#include "fb.h"
#include "ata.h"

Kernel::Kernel()
{
	platform = new Platform();
	video = Video::get();

	if (IDT::get()==NULL) {
		delete platform;
		delete video;
		video = 0;
		platform = 0;
		return;
	}
	IDT::get()->initISR();
	IDT::get()->initIRQ();

	if (Timer::get()==NULL) {
		delete platform;
		delete video;
		video = 0;
		platform = 0;
		return;
	}
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
	if (platform==NULL) return 1;
	if (platform->state()==NULL) return 1;
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
		//p->scanDevices();
	}
	ATA *ata = Platform::ata();
	if (ata!=NULL) {
		ata->init();
		ATA::Device *dev = ata->getDevice();
		while (dev!=NULL) {
			if ((ata->deviceModel(dev) == ATA::STORAGE_SATA || ata->deviceModel(dev) == ATA::STORAGE_PATA) && ata->deviceSize(dev)>0) break;
			dev = ata->nextDevice(dev);
		}
		uint8_t buffer[512];
		for (uint32_t cc=0; cc<512; cc++) {
			buffer[cc] = 0;
		}

		ata->select(dev);
		buffer[0] = 0x42;
		buffer[1] = 0;
		buffer[2] = 0x12;
		buffer[3] = 0x32;
		buffer[4] = 0x24;
		buffer[42] = 0xFE;
		video->printf("Writing...\n");
		if (ata->write(dev, buffer, 1, 0)) {
			video->printf("Done %d.\n", 0);
		}
#if 0
		for (uint32_t sec=0; sec<4; sec++) {
			buffer[1] = (sec%0xFF);
			if (ata->write(dev, buffer, 1,  sec)) {
				//video->printf("Done %d.\n", sec);
			} else {
				video->printf("Fail %d.\n", sec);
			}
		}
#endif
		video->printf("Reading...\n");
		//ata->read(dev, buffer, 1,  0);
		//ata->read(dev, buffer, 1,  2);
		for (uint32_t sec=0; sec<6; sec++) {
			if (ata->read(dev, buffer, 1,  sec)) {
				video->printf("Ok %d. %x %x %x\n", sec, buffer[0], buffer[1], buffer[2]);
			}
		}	
		video->printf("Done.\n");
	}
/*
	for (int i=0; i<0x5FFFFFF; i++) 
		for (int j=0; j<0x22; j++) { }
*/

	FB::ModeConfig conf;
	conf.width=800;
	conf.height=600;
	conf.depth=24;

	//FB::ModeConfig *vconf = platform->fb()->query(NULL);
	if (platform->fb()==NULL) return 1;
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
	video->printf("Done\n");
	while(1) {}

	return 0;
}
