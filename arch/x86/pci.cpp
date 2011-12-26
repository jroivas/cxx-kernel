#include "pci.h"
#include "port.h"
#include "mm.h"
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

PCI::HeaderGeneric *PCI::getHeader(uint32_t bus, uint32_t device)
{
	HeaderGeneric *tmp = (HeaderGeneric*)MM::instance()->alloc(sizeof(struct HeaderGeneric));
	if (tmp==NULL) return NULL;
	tmp->reg00 = getConfig(bus, device, 0, 0x00);
	tmp->reg04 = getConfig(bus, device, 0, 0x04);
	tmp->reg08 = getConfig(bus, device, 0, 0x08);
	tmp->reg0C = getConfig(bus, device, 0, 0x0C);
	tmp->reg10 = getConfig(bus, device, 0, 0x10);
	tmp->reg14 = getConfig(bus, device, 0, 0x14);
	tmp->reg18 = getConfig(bus, device, 0, 0x18);
	tmp->reg1C = getConfig(bus, device, 0, 0x1C);
	tmp->reg20 = getConfig(bus, device, 0, 0x20);
	tmp->reg24 = getConfig(bus, device, 0, 0x24);
	tmp->reg28 = getConfig(bus, device, 0, 0x28);
	tmp->reg2C = getConfig(bus, device, 0, 0x2C);
	tmp->reg30 = getConfig(bus, device, 0, 0x30);
	tmp->reg34 = getConfig(bus, device, 0, 0x34);
	tmp->reg38 = getConfig(bus, device, 0, 0x38);
	tmp->reg3C = getConfig(bus, device, 0, 0x3C);
	return tmp;
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
				//HeaderGeneric *hdr = getHeader(bus, dev);
				Platform::video()->printf("Found device: %4x:%4x\n",v,d);
			}
		}	
	}
	Platform::video()->printf("Scan done.\n");
}
