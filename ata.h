#ifndef _ATA_H
#define _ATA_H

#include "types.h"
#include "pci.h"

class ATA
{
public:
	class Device;

	ATA();
	virtual ~ATA();
	void init();

protected:
	void addDevice(Device *dev);
	PCI *m_pci;
	Device *m_devices;
	virtual void systemPortOut(uint32_t port, uint8_t val) = 0;
	virtual void systemPortOut16(uint32_t port, uint16_t val) = 0;
	virtual void systemPortOut32(uint32_t port, uint32_t val) = 0;
	virtual uint8_t systemPortIn(uint32_t port) = 0;
	virtual uint16_t systemPortIn16(uint32_t port) = 0;
	virtual uint32_t systemPortIn32(uint32_t port) = 0;
};

#endif
