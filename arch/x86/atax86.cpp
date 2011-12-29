#include "atax86.h"
#include "port.h"
#include "../platform.h"

ATAX86::ATAX86() : ATA()
{
}

ATAX86::~ATAX86()
{
}

void ATAX86::systemPortOut(uint32_t port, uint8_t val)
{
	Port::out(port, val);
}

uint8_t ATAX86::systemPortIn(uint32_t port)
{
	return Port::in(port);
}

uint32_t ATAX86::systemPortInLong(uint32_t port)
{
	return Port::inl(port);
}
