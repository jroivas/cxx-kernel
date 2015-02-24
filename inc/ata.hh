#ifndef _ATA_HH
#define _ATA_HH

#include "types.h"
#include "pci.hh"
#include "storage.hh"
#include "filesystem.hh"

class ATA : public Storage
{
public:
    class DevicePrivate;

    ATA();
    virtual ~ATA();
    void init();

#if 0
    uint32_t numDevices();
    Device *getDevice();
    Device *nextDevice(Device *dev);
#endif

    uint32_t deviceSize(Device *d);
    DeviceModel deviceModel(Device *d);
    bool read(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi=0);
    bool write(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi=0);
    bool select(Device *d);

protected:
    //void addDevice(Device *dev);
    PCI *m_pci;
    //DevicePrivate *m_devices;
    virtual void systemPortOut(uint32_t port, uint8_t val) = 0;
    virtual void systemPortOut16(uint32_t port, uint16_t val) = 0;
    virtual void systemPortOut32(uint32_t port, uint32_t val) = 0;
    virtual uint8_t systemPortIn(uint32_t port) = 0;
    virtual uint16_t systemPortIn16(uint32_t port) = 0;
    virtual uint32_t systemPortIn32(uint32_t port) = 0;

    static void interrupt_handler(unsigned int num, void *data);
    unsigned int m_interrupt;
};

class ATAPhys : public FilesystemPhys
{
public:
    ATAPhys(ATA::Device *dev);
    virtual bool read(
        uint8_t *buffer,
        uint32_t sectors,
        uint32_t pos,
        uint32_t pos_hi);
    virtual bool write(
        uint8_t *buffer,
        uint32_t sectors,
        uint32_t pos,
        uint32_t pos_hi);

    virtual uint64_t size() const;
    virtual uint32_t sectorSize() const
    {
        // Hardcoded for now
        return 512;
    }
protected:
    ATA::Device *m_dev;
};

#endif
