#ifndef _ATA_H
#define _ATA_H

#include "types.h"
#include "pci.h"
#include "storage.h"

class ATA : public Storage
{
public:
	class DevicePrivate;

	ATA();
	virtual ~ATA();
	void init();

	uint32_t numDevices();
	Device *getDevice();
	Device *nextDevice(Device *dev);

	uint32_t deviceSize(Device *d);
	DeviceModel deviceModel(Device *d);
	bool read(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi=0);
	bool write(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi=0);
	bool select(Device *d);

protected:
	void addDevice(Device *dev);
	PCI *m_pci;
	DevicePrivate *m_devices;
	virtual void systemPortOut(uint32_t port, uint8_t val) = 0;
	virtual void systemPortOut16(uint32_t port, uint16_t val) = 0;
	virtual void systemPortOut32(uint32_t port, uint32_t val) = 0;
	virtual uint8_t systemPortIn(uint32_t port) = 0;
	virtual uint16_t systemPortIn16(uint32_t port) = 0;
	virtual uint32_t systemPortIn32(uint32_t port) = 0;

	static void interrupt_handler(unsigned int num, void *data);
	unsigned int m_interrupt;
};

#endif
