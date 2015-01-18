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

void ATAX86::systemPortOut16(uint32_t port, uint16_t val)
{
    Port::outw(port, val);
}

void ATAX86::systemPortOut32(uint32_t port, uint32_t val)
{
    Port::outl(port, val);
}

uint8_t ATAX86::systemPortIn(uint32_t port)
{
    return Port::in(port);
}

uint16_t ATAX86::systemPortIn16(uint32_t port)
{
    return Port::inw(port);
}

uint32_t ATAX86::systemPortIn32(uint32_t port)
{
    return Port::inl(port);
}
