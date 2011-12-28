#include "ata.h"
#include "arch/platform.h"

class ATA::Device
{
public:
	Device() { next = NULL; }
	void setup(uint32_t pri, uint32_t sec) { basePort = pri; basePort2 = sec; }
	uint32_t data() { return basePort; }
	uint32_t fetures() { return basePort; }
	uint32_t error() { return basePort+1; }
	uint32_t secCount() { return basePort+2; }
	uint32_t LBA0() { return basePort+3; }
	uint32_t LBA1() { return basePort+4; }
	uint32_t LBA2() { return basePort+5; }
	uint32_t hddDevSel() { return basePort+6; }
	uint32_t command() { return basePort+7; }
	uint32_t status() { return basePort+7; }
	uint32_t altStatus() { return basePort2+2; }
	uint32_t control() { return basePort2+2; }
	uint32_t devAddress() { return basePort2+3; }
	Device *next;

protected:
	uint32_t basePort;
	uint32_t basePort2;
};

ATA::ATA()
{
	m_devices = NULL;
	m_pci = Platform::pci();
	if (m_pci!=NULL) {
		PCI::DeviceIterator *i = m_pci->startIter();
		PCI::HeaderGeneric *hdr = NULL;
		/* Go thorought all the controllers */
		do {
			hdr = m_pci->findNextDevice(i, 0x1, 0x1);
			if (hdr!=NULL) {
				PCI::Header00 *h = (PCI::Header00*)hdr;
				Platform::video()->printf("Addr: %x %x %x %x\n", h->baseAddress0, h->baseAddress1, h->baseAddress2, h->baseAddress3);
				if (h->baseAddress0==0 || h->baseAddress0==1) h->baseAddress0 = 0x1F0;
				if (h->baseAddress1==0 || h->baseAddress1==1) h->baseAddress1 = 0x3F4;
				if (h->baseAddress2==0 || h->baseAddress2==1) h->baseAddress2 = 0x170;
				if (h->baseAddress3==0 || h->baseAddress3==1) h->baseAddress3 = 0x374;

				/* Multiple controller and device support */
				Device *dev1 = new Device();
				Device *dev2 = new Device();
				dev1->setup(h->baseAddress0, h->baseAddress1);
				dev2->setup(h->baseAddress2, h->baseAddress3);
				addDevice(dev1);
				addDevice(dev2);
			}
		} while (hdr!=NULL);
		m_pci->endIter(i);
	}
}

ATA::~ATA()
{
}

void ATA::addDevice(Device *dev)
{
	if (dev==NULL) return;
	if (m_devices==NULL) {
		m_devices = dev;
		return;
	}

	Device *d = m_devices;
	while (d->next != NULL) {
		d = d->next;
	}

	d->next = dev;
}
