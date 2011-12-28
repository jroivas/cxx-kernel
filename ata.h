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

protected:
	void addDevice(Device *dev);
	PCI *m_pci;
	Device *m_devices;
};

#endif
