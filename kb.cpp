#include "kb.h"
#include "types.h"
#include "port.h"
#include "idt.h"
#include "video.h"

static KB *__global_kb = NULL;

KB *KB::get()
{
	if (__global_kb==NULL) {
		__global_kb = new KB();
	}
	return __global_kb;
}

KB::KB()
{
        IDT::get()->installRoutine(1, KB::handler);
}


void KB::run(Regs *r)
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

void KB::handler(Regs *r)
{
        if (__global_kb==NULL) return;
        __global_kb->run(r);
}
