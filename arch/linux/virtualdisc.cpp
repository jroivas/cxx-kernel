#include "virtualdisc.h"
#include "platform.h"
#include <stdio.h>

#define VIRTUAL_SECTOR_SIZE 512

class VirtualDisc::DevicePrivate : public Storage::Device
{
public:
    DevicePrivate() :
        Storage::Device(),
        m_f(NULL),
        m_name("")
    {
        next = NULL;
    }
    ~DevicePrivate();

    bool open(const char *n);
    bool isOk() const
    {
        return (m_f != NULL);
    }

    uint32_t size();
    bool read(uint8_t *buffer, uint16_t sectors, uint32_t addr);
    bool write(uint8_t *buffer, uint16_t sectors, uint32_t addr);

protected:
    FILE *m_f;
    const char *m_name;
};

VirtualDisc::DevicePrivate::~DevicePrivate()
{
    if (m_f != NULL) {
        fclose(m_f);
    }
}

bool VirtualDisc::DevicePrivate::open(const char *n)
{
    m_name = n;
    if (m_f != NULL) return false;

    m_f = fopen(m_name, "r+");
    if (m_f == NULL) return false;

    return true;
}

bool VirtualDisc::DevicePrivate::read(uint8_t *buffer, uint16_t sectors, uint32_t addr)
{
    if (m_f == NULL) return false;

    if (fseek(m_f, VIRTUAL_SECTOR_SIZE*addr, SEEK_SET)!=0) return false;

    if (fread(buffer, VIRTUAL_SECTOR_SIZE, sectors, m_f)>0) return true;

    return false;
}

bool VirtualDisc::DevicePrivate::write(uint8_t *buffer, uint16_t sectors, uint32_t addr)
{
    if (m_f == NULL) return false;

    if (fseek(m_f, VIRTUAL_SECTOR_SIZE*addr, SEEK_SET)!=0) return false;

    if (fwrite(buffer, VIRTUAL_SECTOR_SIZE, sectors, m_f)>0) return true;

    return false;
}

uint32_t VirtualDisc::DevicePrivate::size()
{
    if (m_f == NULL) return 0;

    fseek(m_f, 0, SEEK_END);
    return ftell(m_f);
}

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
    priv->open(name);

    if (priv->isOk()) {
        addDevice(priv);
        return true;
    }
    return false;
}


uint32_t VirtualDisc::deviceSize(VirtualDisc::Device *d)
{
    if (d == NULL) return 0;
    return ((DevicePrivate*)d)->size();
}

VirtualDisc::DeviceModel VirtualDisc::deviceModel(VirtualDisc::Device *d)
{
    (void)d;
    return STORAGE_SATA;
}

bool VirtualDisc::read(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi)
{
    (void)addr_hi;
    if (d == NULL) return false;
    return ((DevicePrivate*)d)->read(buffer, sectors, addr);
}

bool VirtualDisc::write(Device *d, uint8_t *buffer, uint16_t sectors, uint32_t addr, uint32_t addr_hi)
{
    (void)addr_hi;
    if (d == NULL) return false;
    return ((DevicePrivate*)d)->write(buffer, sectors, addr);
}

bool VirtualDisc::select(Device *d)
{
    (void)d;
    return false;
}
