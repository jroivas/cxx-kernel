#include "kbx86.h"
#include "port.h"
#include "idt.h"
#include "video.h"

#define KB_IRQ_NUMBER 33

KBX86::KBX86() : KB()
{
        IDT::get()->installRoutine(KB_IRQ_NUMBER, KBX86::handler);
}

void KBX86::run(Regs *r)
{
        (void)r;
	unsigned char code = Port::in(0x60);

	(void)code;
	// TODO Post code to queue
#if 0
	if (code&0x80) {
	} else {
		Video::get()->putCh(kb[code]);
	}
#endif
	Video::get()->putCh('!');
}
