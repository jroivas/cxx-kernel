#include "ata.h"
#include "arch/platform.h"
#include "timer.h"
#include "arch/x86/port.h"
#include "arch/x86/x86.h"

#define ATA_MODE_LBA 0xE0
#define ATA_MODE_PIO 0xA0

#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY        0xEC

#define ATA_STATUS_ERROR 0x01
#define ATA_STATUS_DRQ   0x08
#define ATA_STATUS_READY 0x40
#define ATA_STATUS_BUSY  0x80

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

class ATA::Device
{
public:
	enum DeviceType { ATAMaster=2, ATASlave=1 };
	enum DeviceModel { UNKNOWN, PATA, PATAPI, SATA, SATAPI };
	Device(ATA *a) { m_ata = a; next = NULL; m_avail = false; m_lba = false; }
	void setup(uint32_t pri, uint32_t sec, DeviceType type) { m_basePort = pri; m_basePort2 = sec; m_type = type; detect(); }
	inline uint32_t data() { return m_basePort; }
	inline uint32_t fetures() { return m_basePort; }
	inline uint32_t error() { return m_basePort+1; }
	inline uint32_t secCount() { return m_basePort+2; }
	inline uint32_t LBA0() { return m_basePort+3; }
	inline uint32_t LBA1() { return m_basePort+4; }
	inline uint32_t LBA2() { return m_basePort+5; }
	inline uint32_t LBA3() { return m_basePort+3; }
	inline uint32_t LBA4() { return m_basePort+4; }
	inline uint32_t LBA5() { return m_basePort+5; }
	inline uint32_t devSel() { return m_basePort+6; }
	inline uint32_t command() { return m_basePort+7; }
	inline uint32_t status() { return m_basePort+7; }
	inline uint32_t altStatus() { return m_basePort2+2; }
	inline uint32_t control() { return m_basePort2+2; }
	inline uint32_t devAddress() { return m_basePort2+3; }
	Device *next;

protected:
	bool reset();
	void wait();
	bool waitStatus();
	void identify();
	void detect();
	void detectModel();
	void detectLBA();

	uint8_t getStatus();
	uint32_t m_basePort;
	uint32_t m_basePort2;
	bool irq;
	bool m_avail;
	bool m_lba;
	uint8_t read(uint32_t port);
	void write(uint32_t port, uint8_t data);
	void readBuffer(uint32_t port, uint32_t *buffer, uint32_t size);
	DeviceType m_type;
	DeviceModel m_model;
	ATA *m_ata;
};

uint8_t ATA::Device::read(uint32_t port)
{
	if (m_ata==NULL) return 0;
	return m_ata->systemPortIn(port);
}

void ATA::Device::write(uint32_t port, uint8_t data)
{
	m_ata->systemPortOut(port, data);
}

void ATA::Device::readBuffer(uint32_t port, uint32_t *buffer, uint32_t size)
{
	uint32_t *ptr = buffer;
	while (size>0) {
		*ptr++ = m_ata->systemPortInLong(port);
		size/=4;
	}
}

bool ATA::Device::reset()
{
	write(status(), 4);
	write(status(), 0);
	wait();
	return waitStatus();
}

void ATA::Device::wait()
{
	// Waiting for 400 ms
	for (int i=0; i<4; i++) {
		read(altStatus());
	}
}

uint8_t ATA::Device::getStatus()
{
	return read(status());
}

bool ATA::Device::waitStatus()
{
	uint8_t stat;
	uint32_t cnt = 0;
	do {
		stat = getStatus();
		if (cnt++>0xFF) return false;
	} while ((stat&ATA_STATUS_BUSY)!=0);
	return true;

#if 0
	do {
		stat = getStatus();
		Platform::video()->printf("2Status: %x\n",stat);
	} while ((stat&ATA_STATUS_READY)==0);
	do {
		stat = getStatus();
	} while ((stat&ATA_STATUS_DRQ)==0);
#endif
}

void ATA::Device::identify()
{
	//Platform::video()->printf("Dev: %x %x\n",devSel(), ATA_MODE_PIO | (m_type<<4));
	write(devSel(), ATA_MODE_PIO | (m_type<<4));
	wait();
	//Timer::get()->msleep(2);

	write(command(), ATA_CMD_IDENTIFY);
	wait();
	//Timer::get()->msleep(2);
}

void ATA::Device::detectLBA()
{
}

void ATA::Device::detectModel()
{
	uint8_t low = read(LBA1());
	uint8_t high = read(LBA2());

	if (low==0x14 && high==0xEB) m_model = PATAPI;
	else if (low==0x69 && high==0x96) m_model = SATAPI;
	else if (low==0x0 && high==0x0) m_model = PATA;
	else if (low==0x3c && high==0xc3) m_model = SATA;
	else m_model = UNKNOWN;
	Platform::video()->printf("Devinfo: %x %x model:%d\n",low,high,m_model);
}

void ATA::Device::detect()
{
	if (m_avail) return;

#if 0
	write(LBA0(), 0xAB);
	uint8_t dt = read(LBA0());
	if (dt!=0xAB) {
		//Platform::video()->printf("Drive not available\n");
		m_avail = false;
		return;
	}
#endif

	write(control(), 2);

	if (!reset()) return;
	identify();

	if (getStatus()==0) {
		m_avail = false;
		return;
	}

	uint8_t error = 0;
	while (1) {
		//TODO: Do we need timeout?
		uint8_t status = getStatus();
		if ((status & ATA_STATUS_ERROR)>0) {
			error = 1;
			break;
		}
		if ((status & ATA_STATUS_BUSY)==0 && (status & ATA_STATUS_DRQ)>0) {
			break;
		}
	}

	detectModel();
	if (m_model==UNKNOWN) return;
	if (error>0) {
		write(command(), ATA_CMD_IDENTIFY_PACKET);
		Timer::get()->msleep(1);
	}

	m_avail = true;

#if 0
	uint8_t buf[2048];
	readBuffer(data(), (uint32_t*)buf, 128);
	Platform::video()->printf("Sig: %d, caps: %d\n",
		*(uint16_t*)(buf+ATA_IDENT_DEVICETYPE),
		*(uint16_t*)(buf+ATA_IDENT_CAPABILITIES)
		);

	Platform::video()->printf("Size: %u vs. %u\n",
		*(uint32_t*)(buf+ATA_IDENT_MAX_LBA),
		*(uint32_t*)(buf+ATA_IDENT_MAX_LBA_EXT)
		);
#endif
}

ATA::ATA()
{
	m_devices = NULL;
	m_pci = Platform::pci();
}

ATA::~ATA()
{
}

void ATA::init()
{
	if (m_pci!=NULL) {
		PCI::DeviceIterator *i = m_pci->startIter();
		PCI::HeaderGeneric *hdr = NULL;
		/* Go thorought all the controllers */
		do {
			hdr = m_pci->findNextDevice(i, 0x1, 0x1);
			if (hdr!=NULL && (((PCI::HeaderCommon*)hdr)->headerType&0x1F)==0) {
				m_pci->set(i, 0x3C, 0xFE);
				hdr = m_pci->getCurrent(i);
				PCI::Header00 *h = (PCI::Header00*)hdr;
				uint16_t cmd = h->common.command;
				cmd &= ~(1<<10);
				cmd |= ((1<<0)|(1<<2));
				Platform::video()->printf("newcmd: %x cmd: %x\n",cmd, h->common.command);
				if (cmd!=h->common.command) {
					m_pci->set(i, 0x4, cmd&0xFF);
				}
				hdr = m_pci->getCurrent(i);
				h = (PCI::Header00*)hdr;
				Platform::video()->printf("Int: %x cmd: %x\n",h->interruptLine, h->common.command);

				Platform::video()->printf("Addr: %x %x %x %x\n", h->baseAddress0, h->baseAddress1, h->baseAddress2, h->baseAddress3);
#if 1
				if (h->baseAddress0==0 || h->baseAddress0==1) h->baseAddress0 = 0x1F0;
				else h->baseAddress0 &= 0xFFFFFFFC;
				if (h->baseAddress1==0 || h->baseAddress1==1) h->baseAddress1 = 0x3F4;
				else h->baseAddress1 &= 0xFFFFFFFC;
				if (h->baseAddress2==0 || h->baseAddress2==1) h->baseAddress2 = 0x170;
				else h->baseAddress2 &= 0xFFFFFFFC;
				if (h->baseAddress3==0 || h->baseAddress3==1) h->baseAddress3 = 0x374;
				else h->baseAddress3 &= 0xFFFFFFFC;
				Platform::video()->printf("Addr: %x %x %x %x\n", h->baseAddress0, h->baseAddress1, h->baseAddress2, h->baseAddress3);
#endif

				/* Multiple controller and device support */
				Device *dev1 = new Device(this);
				Device *dev2 = new Device(this);
				Device *dev3 = new Device(this);
				Device *dev4 = new Device(this);
				dev1->setup(h->baseAddress0, h->baseAddress1, ATA::Device::ATAMaster);
				dev2->setup(h->baseAddress0, h->baseAddress1, ATA::Device::ATASlave);
				dev3->setup(h->baseAddress2, h->baseAddress3, ATA::Device::ATAMaster);
				dev4->setup(h->baseAddress2, h->baseAddress3, ATA::Device::ATASlave);
				addDevice(dev1);
				addDevice(dev2);
				addDevice(dev3);
				addDevice(dev4);
			}
		} while (hdr!=NULL);
		m_pci->endIter(i);
	}
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
