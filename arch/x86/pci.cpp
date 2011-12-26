#include "pci.h"
#include "port.h"
#include "../platform.h"

#define PCI_PORT_CONFIG_ADDR 0xCF8
#define PCI_PORT_CONFIG_DATA 0xCFC
#define PCI_CONFIG_GET_DEVICE(x) ((x>>16)&0xFFFF)
#define PCI_CONFIG_GET_VENDOR(x) (x&0xFFFF)
#define PCI_CONFIG_GET_HEADER_TYPE(x) ((x>>16)&0xF)
#define PCI_CONFIG_IS_MULTI_FUNCTION(x) (((x>>16)&0xF0)==0x80)

#define PCI_REGISTER_ID 0x0
#define PCI_REGISTER_HEADER_TYPE 0xC

PCI::PCI()
{
}

PCI::~PCI()
{
}

uint32_t PCI::getConfig(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg)
{
	uint32_t addr = 0;

	addr = 1<<31; 
	addr |= ((bus & 0xFF) << 16); 
	addr |=  ((device & 0x1F) << 11);
	addr |=  ((func & 0x7) << 8);
	addr |=  (reg & 0xFC);

	Port::outl(PCI_PORT_CONFIG_ADDR, addr);
	return Port::inl(PCI_PORT_CONFIG_DATA);
}

uint32_t PCI::getVendor(uint32_t bus, uint32_t device)
{
	return PCI_CONFIG_GET_VENDOR(getConfig(bus, device, 0, PCI_REGISTER_ID));
}

uint32_t PCI::getDevice(uint32_t bus, uint32_t device)
{
	if (getVendor(bus, device)!=0xFFFF) {
		return PCI_CONFIG_GET_DEVICE(getConfig(bus, device, 0, PCI_REGISTER_ID));
	}
	return 0xFFFF;
}

uint32_t PCI::getHeaderType(uint32_t bus, uint32_t device)
{
	return PCI_CONFIG_GET_HEADER_TYPE(getConfig(bus, device, 0, PCI_REGISTER_HEADER_TYPE));
}

void PCI::scanDevices()
{
	Platform::video()->printf("Scanning PCI...\n");
	for (uint32_t bus = 0; bus <= 0xFF; bus++) {
		for (uint32_t dev = 0; dev <= 0x1f; dev++) {
			uint32_t res = getConfig(bus, dev, 0, 0);
			uint32_t v; 
			if ((v=PCI_CONFIG_GET_VENDOR(res))!=0xFFFF) {
				uint32_t d = PCI_CONFIG_GET_DEVICE(res);
				Platform::video()->printf("Found device: %x:%x   %x\n",v,d,getHeaderType(bus,dev));
			}
		}	
	}
	Platform::video()->printf("Scan done.\n");
}
