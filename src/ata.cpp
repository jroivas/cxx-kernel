#include "ata.h"
#include "arch/platform.h"
#include "timer.h"
#include "arch/x86/port.h"
#include "arch/x86/x86.h"

#if 0
#define ATA_MODE_LBA 0xE0
#define ATA_MODE_PIO 0xA0
#else
#define ATA_MODE_LBA (1<<6)
#define ATA_MODE_PIO (0)
#endif
#define ATA_DEVSEL_CONST (1<<5|1<<7)
//#define ATA_DEVSEL_CONST 0

#define ATA_CMD_READ            0x20
#define ATA_CMD_READ_48         0x24
#define ATA_CMD_READ_DMA        0xC8
#define ATA_CMD_READ_DMA_48     0x25
#define ATA_CMD_WRITE           0x30
#define ATA_CMD_WRITE_48        0x34
#define ATA_CMD_WRITE_DMA       0xCA
#define ATA_CMD_WRITE_DMA_48    0x35
#define ATA_CMD_FLUSH           0xE7
#define ATA_CMD_FLUSH_48        0xEA
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY        0xEC

#define ATA_STATUS_ERROR 0x01
#define ATA_STATUS_DRQ   0x08
#define ATA_STATUS_DF    0x20
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

#define ATA_MAX_WAIT_CNT 0xFF

#define ATA_INTERRUPT_BASE 14

void ATA::interrupt_handler(unsigned int num, void *data)
{
	(void)num;
	(void)data;
	Platform::video()->printf("ATA interrupt\n");
}

class ATA::DevicePrivate : public Storage::Device
{
public:
	enum DeviceType { ATAMaster=0, ATASlave=1 };

	DevicePrivate(ATA *a) : Device() { m_ata = a; next = NULL; m_avail = false; m_lba = false; m_lba48 = false; m_dma = false; m_class = CLASS_ATA; }
	void setup(uint32_t pri, uint32_t sec, DeviceType type) { m_basePort = pri; m_basePort2 = sec; m_type = type; detect(); }
	//DevicePrivate *next;
	bool readSector(uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi=0);
	bool writeSector(uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi=0);
	uint32_t size() { return m_size; }
	DeviceModel model() { return m_model; }
	void select(uint8_t head=0);

protected:
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

	bool reset();
	void wait();
	bool waitStatus(uint8_t extra=0, uint8_t extra_val=0);
	bool identify();
	void detect();
	void detectModel();
	bool prepareAccess(uint16_t sectors, uint32_t addr, uint32_t addr_hi=0);
	bool poll(bool extra=false);

	inline void setHi() { write(control(), (0x80+m_irq)?0:0x2); }
	inline void setLo() { write(control(), m_irq?0:0x2); }

	uint8_t getStatus();
	uint32_t getSecCount();
	uint32_t m_basePort;
	uint32_t m_basePort2;
	uint32_t m_size;
	bool m_irq;
	bool m_avail;
	bool m_lba;
	bool m_lba48;
	bool m_dma;
	uint8_t read(uint32_t port);
	void write(uint32_t port, uint8_t data);
	void write16(uint32_t port, uint16_t data);
	void readBuffer(uint32_t port, uint32_t *buffer, uint32_t size);
	void writeBuffer(uint32_t port, uint32_t *buffer, uint32_t size);
	DeviceType m_type;
	DeviceModel m_model;
	ATA *m_ata;
	uint8_t m_mode;

	uint16_t m_cyl;
	uint8_t m_sect;
	uint8_t m_head;
};

uint8_t ATA::DevicePrivate::read(uint32_t port)
{
	if (m_ata==NULL) return 0;
	return m_ata->systemPortIn(port);
}

void ATA::DevicePrivate::write(uint32_t port, uint8_t data)
{
	m_ata->systemPortOut(port, data);
}

void ATA::DevicePrivate::write16(uint32_t port, uint16_t data)
{
	m_ata->systemPortOut16(port, data);
}

uint32_t ATA::DevicePrivate::getSecCount()
{
	return m_ata->systemPortIn16(secCount());
}

void ATA::DevicePrivate::readBuffer(uint32_t port, uint32_t *buffer, uint32_t size)
{
	uint32_t *ptr = buffer;
	while (size>0) {
/*
		*ptr++ = m_ata->systemPortIn32(port);
		size-=4;
*/
		uint16_t a = m_ata->systemPortIn16(port);
		uint16_t b = m_ata->systemPortIn16(port);
		*ptr++ = a + (b<<16);
		size-=4;
	}
}

void ATA::DevicePrivate::writeBuffer(uint32_t port, uint32_t *buffer, uint32_t size)
{
	uint32_t *ptr = buffer;
	while (size>0) {
		m_ata->systemPortOut32(port, *ptr++);
		size-=4;
	}
}

bool ATA::DevicePrivate::reset()
{
#if 1
	write(status(), 4);
	write(status(), 0);
#endif
	wait();
	return waitStatus();
}

void ATA::DevicePrivate::wait()
{
	for (int i=0; i<5; i++) {
		read(altStatus());
	}
	Timer::get()->msleep(1);
}

uint8_t ATA::DevicePrivate::getStatus()
{
	return read(status());
}

bool ATA::DevicePrivate::waitStatus(uint8_t extra, uint8_t extra_val)
{
	uint8_t stat;
	uint32_t cnt = 0;
	do {
		stat = getStatus();
		if (cnt++>ATA_MAX_WAIT_CNT) return false;
	} while ((stat&ATA_STATUS_BUSY)!=0);

	if (extra>0) {
		cnt = 0;
		do {
			stat = getStatus();
			if (cnt++>ATA_MAX_WAIT_CNT) return false;
			wait();
		} while ((stat&extra)!=extra_val);
	}

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

void ATA::DevicePrivate::select(uint8_t head)
{
	//Platform::video()->printf("Devsel: %x\n", ATA_DEVSEL_CONST | m_mode | (m_type<<4) | head);
	write(devSel(), ATA_DEVSEL_CONST | m_mode | (m_type<<4) | head);
	wait();
}

bool ATA::DevicePrivate::identify()
{
	select();

	write(command(), ATA_CMD_IDENTIFY);
	wait();

	uint8_t status = getStatus();
	if (status==0xFF || status==0x7f) return false;
	return true;
}

void ATA::DevicePrivate::detectModel()
{
	uint8_t low = read(LBA1());
	uint8_t high = read(LBA2());

	if (low==0x14 && high==0xEB) m_model = STORAGE_PATAPI;
	else if (low==0x69 && high==0x96) m_model = STORAGE_SATAPI;
	else if (low==0x0 && high==0x0) m_model = STORAGE_PATA;
	else if (low==0x3c && high==0xc3) m_model = STORAGE_SATA;
	else m_model = STORAGE_UNKNOWN;
	//Platform::video()->printf("Devinfo: %x %x model:%d\n",low,high,m_model);
}

bool ATA::DevicePrivate::prepareAccess(uint16_t sectors, uint32_t addr, uint32_t addr_hi)
{
	uint8_t sect = 0;
	uint16_t cyl = 0;
	uint8_t head = 0;
	uint8_t io_addr[6];
	m_lba48 = false;

	if (!reset()) return false;
	if (addr>=0x10000000 || addr_hi>0) { // Need LBA48
		io_addr[0] = (addr & 0xFF);
		io_addr[1] = (addr >> 8) & 0xFF;
		io_addr[2] = (addr >> 16) & 0xFF;
		io_addr[3] = (addr >> 24) & 0xFF;
		io_addr[4] = (addr_hi) & 0xFF;
		io_addr[5] = (addr_hi >> 8) & 0xFF;
		head = 0;
		m_lba48 = true;
	}
	else if (m_lba) {
		io_addr[0] = (addr & 0xFF);
		io_addr[1] = (addr >> 8) & 0xFF;
		io_addr[2] = (addr >> 16) & 0xFF;
		io_addr[3] = 0;
		io_addr[4] = 0;
		io_addr[5] = 0;
		head = (addr & 0xF000000) >> 24;
	} else {
		//Platform::video()->printf("WARNING: CHS\n");
		sect = (addr % m_sect) + 1;
		cyl = (addr + 1 - sect) / (m_head * m_sect);
		head = (addr + 1 - sect) % (m_head * m_sect) / m_sect;
		io_addr[0] = sect;
		io_addr[1] = cyl & 0xFF;
		io_addr[2] = (cyl >> 8) & 0xFF;
		io_addr[3] = 0;
		io_addr[4] = 0;
		io_addr[5] = 0;
	}


	if (!waitStatus()) return false;

	select(head);

	if (!waitStatus()) return false;


	//if (!waitStatus(ATA_STATUS_DRQ, ATA_STATUS_DRQ)) 
	//	select(head);

	//if (!waitStatus(ATA_STATUS_DRQ, ATA_STATUS_DRQ)) return false;

	if (m_lba48) {
		setHi(); write(secCount(), 0); setLo();
		setHi(); write(LBA3(), io_addr[3]); setLo();
		setHi(); write(LBA4(), io_addr[4]); setLo();
		setHi(); write(LBA5(), io_addr[5]); setLo();
	}
	setLo();
	write(secCount(), sectors);
	write(LBA0(), io_addr[0]);
	write(LBA1(), io_addr[1]);
	write(LBA2(), io_addr[2]);

	return true;
}

bool ATA::DevicePrivate::poll(bool extra)
{
	wait();

	if (!waitStatus()) return false;

	if (extra) {
		uint8_t status = getStatus();
		if ((status & ATA_STATUS_ERROR)>0) return false;
		if ((status & ATA_STATUS_DF)>0) return false;
		if ((status & ATA_STATUS_DRQ)==0) return false;
	}

	return true;
}

bool ATA::DevicePrivate::readSector(uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi)
{
	uint8_t cmd = 0;
	if (buffer==NULL) return false;

	if (!prepareAccess(sectors, addr, addr_hi)) return false;

	if (m_lba48 && m_dma) cmd = ATA_CMD_READ_DMA_48;
	else if (m_dma) cmd = ATA_CMD_READ_DMA;
	else if (m_lba48 && !m_dma) cmd = ATA_CMD_READ_48;
	else cmd = ATA_CMD_READ;

	write(command(), cmd);

	if (!waitStatus(ATA_STATUS_ERROR, 0)) return false;
	if (!waitStatus(ATA_STATUS_DRQ, ATA_STATUS_DRQ)) return false;

	uint8_t *tmp = buffer;
	if (!m_dma) {
		for (uint32_t i=0; i<sectors; i++) {
			if (!poll(true)) return false;

			readBuffer(data(), (uint32_t*)tmp, 512);
			tmp+=512;
		}
		wait();
	}

	return true;
}

/* buffer needs to be sectors*SECTOR_SIZE
 */
bool ATA::DevicePrivate::writeSector(uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi)
{
	uint8_t cmd = 0;
	if (buffer==NULL) return false;

	if (!prepareAccess(sectors, addr, addr_hi)) return false;

	if (m_lba48 && m_dma) cmd = ATA_CMD_WRITE_DMA_48;
	else if (m_dma) cmd = ATA_CMD_WRITE_DMA;
	else if (m_lba48 && !m_dma) cmd = ATA_CMD_WRITE_48;
	else cmd = ATA_CMD_WRITE;

	write(command(), cmd);

	uint8_t *tmp = buffer;
	if (!m_dma) {
		for (uint32_t i=0; i<sectors; i++) {
			if (!poll()) return false;

			writeBuffer(data(), (uint32_t*)tmp, 512);
			tmp+=512;
		}
		cmd = 0;
		if (m_lba48) cmd = ATA_CMD_FLUSH_48;
		else cmd = ATA_CMD_FLUSH;
		write(command(), cmd);
		poll();
	}

	return true;
}

void ATA::DevicePrivate::detect()
{
	if (m_avail) return;

	m_mode = ATA_MODE_PIO;
	//m_mode = ATA_MODE_LBA;

	//write(control(), 2);
	write(control(), m_irq?0:0x2);

	if (!reset()) return;
	if (!identify()) {
		m_avail = false;
		return;
	}
#if 0
	write(LBA0(), 0xAB);
	uint8_t dt = read(LBA0());
	if (dt!=0xAB) {
		//Platform::video()->printf("Drive not available\n");
		m_avail = false;
		return;
	}
#endif

	uint8_t error = 0;
	uint32_t errcnt = 0;
	while (1) {
		uint8_t status = getStatus();
		if ((status & ATA_STATUS_ERROR)>0) {
			error = 1;
			break;
		}
		if ((status & ATA_STATUS_BUSY)==0 && (status & ATA_STATUS_DRQ)>0) {
			break;
		}
		if (errcnt++>ATA_MAX_WAIT_CNT) {
			m_avail = false;
			return;
		}
	}

	detectModel();
	if (m_model==STORAGE_UNKNOWN) return;
	if (error>0) {
		write(command(), ATA_CMD_IDENTIFY_PACKET);
		Timer::get()->msleep(1);
	}

	uint8_t buf[2048];
	uint8_t devname[42];
	readBuffer(data(), (uint32_t*)buf, 128);

	for (uint32_t tt=0; tt<40; tt+=2) {
		devname[tt] = *(buf+ATA_IDENT_MODEL+tt+1);
		devname[tt+1] = *(buf+ATA_IDENT_MODEL+tt);
		
	}
	devname[40]=0;
	Platform::video()->printf("Model: %s\n",devname);

	if ((*(uint16_t*)(buf+ATA_IDENT_CAPABILITIES)&0x200)==0x200) {
		m_lba = true;
		m_mode = ATA_MODE_LBA;
		//Platform::video()->printf("Has LBA\n");
	}

	if (!m_lba) {
		m_sect = (*(uint8_t*)(buf+ATA_IDENT_SECTORS));
		m_cyl = (*(uint16_t*)(buf+ATA_IDENT_CYLINDERS));
		m_head = (*(uint8_t*)(buf+ATA_IDENT_HEADS));
		if (m_sect>0) m_size = (((m_cyl*m_head+m_head)*m_sect+(m_sect-1))*512);
		else m_size = 0;
		//Platform::video()->printf("Size: sec %u cyl %u head %u == %u (%u MB)\n",sec,cyl,head,m_size,m_size/1024/1024);
	} else {
		m_size = *(uint32_t*)(buf+ATA_IDENT_MAX_LBA)*512;
	}
	Platform::video()->printf("Size: %u (%u MB)\n",
		m_size,
		m_size/1024/1024
		);

	m_avail = true;
}

ATA::ATA()
{
	m_devices = NULL;
	m_pci = Platform::pci();
	m_interrupt = ATA_INTERRUPT_BASE;
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
				//Platform::video()->printf("newcmd: %x cmd: %x\n",cmd, h->common.command);
				if (cmd!=h->common.command) {
					m_pci->set(i, 0x4, cmd&0xFF);
				}
				hdr = m_pci->getCurrent(i);
				h = (PCI::Header00*)hdr;
				//Platform::video()->printf("Int: %x cmd: %x\n",h->interruptLine, h->common.command);

				//Platform::video()->printf("Addr: %x %x %x %x\n", h->baseAddress0, h->baseAddress1, h->baseAddress2, h->baseAddress3);
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
				DevicePrivate *dev1 = new DevicePrivate(this);
				DevicePrivate *dev2 = new DevicePrivate(this);
				DevicePrivate *dev3 = new DevicePrivate(this);
				DevicePrivate *dev4 = new DevicePrivate(this);
				dev1->setup(h->baseAddress0, h->baseAddress1, ATA::DevicePrivate::ATAMaster);
				dev2->setup(h->baseAddress0, h->baseAddress1, ATA::DevicePrivate::ATASlave);
				dev3->setup(h->baseAddress2, h->baseAddress3, ATA::DevicePrivate::ATAMaster);
				dev4->setup(h->baseAddress2, h->baseAddress3, ATA::DevicePrivate::ATASlave);
				addDevice(dev1);
				addDevice(dev2);
				addDevice(dev3);
				addDevice(dev4);

				Platform::idt()->installHandler(m_interrupt++, interrupt_handler, NULL, NULL);
			}
		} while (hdr!=NULL);
		m_pci->endIter(i);
	}
}

uint32_t ATA::deviceSize(Device *d)
{
	if (d==NULL) return 0;

	return ((DevicePrivate*)d)->size();
}

ATA::DeviceModel ATA::deviceModel(Device *d)
{
	if (d==NULL) return STORAGE_UNKNOWN;

	return ((DevicePrivate*)d)->model();
}

bool ATA::read(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi)
{
	if (d==NULL) return false;

	return ((DevicePrivate*)d)->readSector(buffer, sectors, addr, addr_hi);
}

bool ATA::write(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi)
{
	if (d==NULL) return false;

	return ((DevicePrivate*)d)->writeSector(buffer, sectors, addr, addr_hi);
}

bool ATA::select(Device *d)
{
	
	if (d==NULL) return false;

	uint8_t tmp[512];
	((DevicePrivate*)d)->select();

 	//FIXME hack to get reading/writing right. For some reason first read/write does nothing. Sequent read/write works just fine.
	((DevicePrivate*)d)->readSector(tmp, 1, 0);

	return true;
}
