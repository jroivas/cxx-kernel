#include "pcix86.h"

#include "../platform.h"
#include "port.h"

#define PCI_PORT_CONFIG_ADDR 0xCF8
#define PCI_PORT_CONFIG_DATA 0xCFC

PCIX86::PCIX86() : PCI()
{
}

PCIX86::~PCIX86()
{
}

void PCIX86::systemPut(uint32_t val)
{
	Port::outl(PCI_PORT_CONFIG_ADDR, val);
}

uint32_t PCIX86::systemGet()
{
	return Port::inl(PCI_PORT_CONFIG_DATA);
}
