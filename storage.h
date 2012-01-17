#ifndef STORAGE_H
#define STORAGE_H

class Storage
{
public:
	enum DeviceModel { STORAGE_UNKNOWN, STORAGE_PATA, STORAGE_PATAPI, STORAGE_SATA, STORAGE_SATAPI, STORAGE_USB };
	enum DeviceClass { CLASS_NONE, CLASS_ATA, CLASS_USB };
	class Device { 
	public:
		Device() { m_class = CLASS_NONE; }
		uint8_t m_class;
	};

	Storage() {}
	virtual ~Storage() {}

	virtual uint32_t numDevices() = 0;
	virtual Device *getDevice() = 0;
	virtual Device *nextDevice(Device *dev) = 0;

	virtual uint32_t deviceSize(Device *d) = 0;
	virtual DeviceModel deviceModel(Device *d) = 0;
	virtual bool read(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi=0) = 0;
	virtual bool write(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi=0) = 0;
	virtual bool select(Device *d) = 0;

protected:
};
#endif
