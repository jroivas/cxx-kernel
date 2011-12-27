#include "pci.h"
#include "mm.h"
#include "arch/platform.h"

#define PCI_CONFIG_GET_DEVICE(x) ((x>>16)&0xFFFF)
#define PCI_CONFIG_GET_VENDOR(x) (x&0xFFFF)
#define PCI_CONFIG_GET_HEADER_TYPE(x) ((x)&0x1F)
#define PCI_CONFIG_IS_MULTI(x) ((x&0x80)==0x80)

#define PCI_REGISTER_ID 0x0
#define PCI_REGISTER_HEADER_TYPE 0xC

#define PCI_HEADER_TO_00(x) ((PCI::Header00*)x)
#define PCI_HEADER_TO_01(x) ((PCI::Header01*)x)
#define PCI_TYPE_FROM_GENERIC_HEADER(x) (((HeaderCommon*)x)->headerType)

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
	addr |= ((device & 0x1F) << 11);
	addr |= ((func & 0x7) << 8);
	addr |= (reg & 0xFC);

	//Platform::video()->printf("%2x %2x %2x %2x ADDR: %8x\n",bus, device, func, reg, addr);

	//systemPut(PCI_PORT_CONFIG_ADDR, addr);
	systemPut(addr);
	return systemGet();
}

bool PCI::isAvailable()
{
	if (systemGet()==0xFFFFFFFF) return false;
	return true;
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

PCI::HeaderGeneric *PCI::getHeader(uint32_t bus, uint32_t device)
{
	HeaderGeneric *tmp = (HeaderGeneric*)MM::instance()->alloc(sizeof(struct HeaderGeneric));
	if (tmp==NULL) return NULL;
	for (int i=0; i<16; i++) {
		tmp->reg[i] = getConfig(bus, device, 0, i*4);
	}
	return tmp;
}

bool PCI::isDevice(uint32_t bus, uint32_t device)
{
	if (getVendor(bus,device)==0xFFFF) return false;
	return true;
}

void PCI::scanDevices()
{
	int32_t addr;
	addr = 1<<31; 
	addr |= ((0x1 & 0xFF) << 16); 
	addr |= ((0xA & 0x1F) << 11);
	addr |= ((0x7 & 0x7) << 8);
	addr |= (0 & 0xFC);
	Platform::video()->printf("Preaddr: %8x\n",addr);

	Platform::video()->printf("Scanning PCI...\n");
	for (uint32_t bus = 0; bus < 0x100; bus++) {
		for (uint32_t dev = 0; dev < 0x20; dev++) {
			bool multi = false;
			for (uint32_t func = 0; func < 0x8; func++) {
				if (func>0 && !multi) break;
				uint32_t res = getConfig(bus, dev, func, 0);
				uint32_t v; 
				if ((v=PCI_CONFIG_GET_VENDOR(res))!=0xFFFF) {
					uint32_t d = PCI_CONFIG_GET_DEVICE(res);
					HeaderGeneric *hdr = getHeader(bus, dev);
					//Platform::video()->printf("Found device: %4x:%4x  class: %2x, %2x   %x %d\n",v,d,((HeaderCommon*)hdr)->classCode, ((HeaderCommon*)hdr)->subclass, ((HeaderCommon*)hdr)->headerType, PCI_CONFIG_IS_MULTI(((HeaderCommon*)hdr)->headerType));
					Platform::video()->printf("PCI %2x:%2x.%d: %4x:%4x  class: %2x, %2x   headerType: %2x\n",bus,dev,func,v,d,
						((HeaderCommon*)hdr)->classCode, ((HeaderCommon*)hdr)->subclass,
						((HeaderCommon*)hdr)->headerType
						);
					#if 0 
					if (PCI_CONFIG_GET_HEADER_TYPE(((HeaderCommon*)hdr)->headerType)==0) {
						Header00 *h = (Header00*)hdr;
						Platform::video()->printf(" h0: %4x:%4x\n", h->subsystemVendorID,h->subsystemID);
					} 
					else if (PCI_CONFIG_GET_HEADER_TYPE(((HeaderCommon*)hdr)->headerType)==1) {
						Header01 *h = (Header01*)hdr;
					}
					#endif

					//if (!PCI_CONFIG_IS_MULTI(((HeaderCommon*)hdr)->headerType)) break;
					if (PCI_CONFIG_IS_MULTI(((HeaderCommon*)hdr)->headerType)) multi = true;
				}
			}
		}	
	}
	Platform::video()->printf("Scan done.\n");
}
