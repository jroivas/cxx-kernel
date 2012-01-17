#include "storage.h"

void Storage::addDevice(Device *dev)
{
	if (dev==NULL) return;
	if (m_devices==NULL) {
		m_devices = (Device*)dev;
		return;
	}

	Device*d = m_devices;
	while (d->next != NULL) {
		d = d->next;
	}

	d->next = dev;
}

uint32_t Storage::numDevices()
{
	Device *d = m_devices;
	if (d==NULL) return 0;

	uint32_t n = 1;

	while (d->next != NULL) {
		n++;
	}
	return n;
}

Storage::Device *Storage::getDevice()
{
	return m_devices;
}

Storage::Device *Storage::nextDevice(Device *dev)
{
	if (dev==NULL) return NULL;

	return dev->next;
}
