#ifndef VIRTUALDISK_H
#define VIRTUALDISK_H

#include "storage.h"

class VirtualDisc : public Storage
{
public:
	class DevicePrivate;

	VirtualDisc();
	~VirtualDisc();

	bool append(const char *name);

	uint32_t deviceSize(Device *d);
	DeviceModel deviceModel(Device *d);
	bool read(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi=0);
	bool write(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi=0);
	bool select(Device *d);

protected:

};

#endif
