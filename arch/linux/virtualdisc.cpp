#include "virtualdisc.h"

class VirtualDisc::DevicePrivate : public Storage::Device
{
public:
	DevicePrivate() : Storage::Device() { next = NULL; m_name = ""; }
	~DevicePrivate() { }

	const char *m_name;
};

VirtualDisc::VirtualDisc()
{
	m_devices = NULL;
}

VirtualDisc::~VirtualDisc()
{
}

bool VirtualDisc::append(const char *name)
{
	DevicePrivate *priv = new DevicePrivate();
	priv->m_name = name;

	addDevice(priv);
	return true;
}


uint32_t VirtualDisc::deviceSize(VirtualDisc::Device *d)
{
	(void)d;
	return 0;
}

VirtualDisc::DeviceModel VirtualDisc::deviceModel(VirtualDisc::Device *d)
{
	(void)d;
	return STORAGE_UNKNOWN;
}

bool VirtualDisc::read(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi)
{
	(void)d;
	(void)buffer;
	(void)sectors;
	(void)addr;
	(void)addr_hi;
	return false;
}

bool VirtualDisc::write(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi)
{
	(void)d;
	(void)buffer;
	(void)sectors;
	(void)addr;
	(void)addr_hi;
	return false;
}

bool VirtualDisc::select(Device *d)
{
	(void)d;
	return false;
}
