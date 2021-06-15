#include "pci.hh"
#include "mm.h"
#include "arch/platform.h"

#define PCI_CONFIG_GET_DEVICE(x) ((x>>16)&0xFFFF)
#define PCI_CONFIG_GET_VENDOR(x) (x&0xFFFF)
#define PCI_CONFIG_GET_HEADER_TYPE(x) ((x)&0x1F)
#define PCI_CONFIG_IS_MULTI(x) ((x&0x80)==0x80)

#define PCI_REGISTER_ID 0x0
#define PCI_REGISTER_HEADER_TYPE 0xC

#define PCI_HEADER_TO_00(x) ((PCI::Header00*)x)
#define PCI_HEADER_TO_01(x) ((PCI::Header01*)x)
#define PCI_TYPE_FROM_GENERIC_HEADER(x) (((HeaderCommon*)x)->headerType)

class PCI::DeviceIterator
{
public:
    DeviceIterator()
        : bus(0),
        dev(0),
        func(0)
    {
    }
    uint32_t bus;
    uint32_t dev;
    uint32_t func;
};

PCI::PCI()
{
}

PCI::~PCI()
{
}

uint32_t PCI::getPciAddr(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg)
{
    uint32_t addr = 1 << 31;
    addr |= ((bus & 0xFF) << 16);
    addr |= ((device & 0x1F) << 11);
    addr |= ((func & 0x7) << 8);
    addr |= (reg & 0xFC);

    return addr;
}

uint32_t PCI::getConfig(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg)
{
    systemPut(getPciAddr(bus, device, func, reg));
    return systemGet();
}

void PCI::setConfig(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg, uint32_t val)
{
    systemPut(getPciAddr(bus, device, func, reg));
    systemPutData(val);
}

uint32_t PCI::readValue32(HeaderGeneric *hdr, uint32_t addr)
{
    if (!hdr)
        return 0;
    HeaderCommon *h = (HeaderCommon*)hdr;
    Platform::video()->printf("Read %x %x %x\n", h->bus, h->dev, h->func);
    return getConfig(h->bus, h->dev, h->func, addr);
}

bool PCI::isAvailable()
{
    return (systemGet() != 0xFFFFFFFF);
}

uint32_t PCI::getVendor(uint32_t bus, uint32_t device, uint32_t func)
{
    return PCI_CONFIG_GET_VENDOR(getConfig(bus, device, func, PCI_REGISTER_ID));
}

uint32_t PCI::getDevice(uint32_t bus, uint32_t device, uint32_t func)
{
    if (getVendor(bus, device, func) != 0xFFFF) {
        return PCI_CONFIG_GET_DEVICE(getConfig(bus, device, func, PCI_REGISTER_ID));
    }
    return 0xFFFF;
}

PCI::HeaderGeneric *PCI::getHeader(uint32_t bus, uint32_t device, uint32_t func)
{
    HeaderGeneric *tmp = (HeaderGeneric*)MM::instance()->alloc(sizeof(struct HeaderGeneric));
    if (tmp == nullptr) {
        return nullptr;
    }
    for (int i = 0; i < 16; i++) {
        tmp->reg[i] = getConfig(bus, device, func, i * 4);
    }
#if 1
    tmp->reg[16] = bus;
    tmp->reg[17] = device;
    tmp->reg[18] = func;
#else
    tmp->reg[16] = 0xdeadbeef;
    tmp->reg[17] = 0xcafebabe;
    tmp->reg[18] = 0x1337c0de;
#endif
    return tmp;
}

void PCI::setHeader(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg, uint8_t val)
{
    setConfig(bus, device, func, reg, val);
}

void PCI::set(DeviceIterator *iter, uint32_t reg, uint8_t val)
{
    if (iter == nullptr) return;
    setHeader(iter->bus, iter->dev, iter->func, reg, val);
}

bool PCI::isDevice(uint32_t bus, uint32_t device, uint32_t func)
{
    return (getVendor(bus,device,func) != 0xFFFF);
}

void PCI::scanDevices()
{
    int32_t addr;
    addr = 1 << 31;
    addr |= ((0x1 & 0xFF) << 16);
    addr |= ((0xA & 0x1F) << 11);
    addr |= ((0x7 & 0x7) << 8);
    addr |= (0 & 0xFC);
    //Platform::video()->printf("Preaddr: %8x\n",addr);

    if (m_verbose) Platform::video()->printf("Scanning PCI...\n");
    for (uint32_t bus = 0; bus < 0x100; bus++) {
        for (uint32_t dev = 0; dev < 0x20; dev++) {
            bool multi = false;
            for (uint32_t func = 0; func < 0x8; func++) {
                if (func > 0 && !multi) break;
                uint32_t res = getConfig(bus, dev, func, 0);
                uint32_t v;
                if ((v = PCI_CONFIG_GET_VENDOR(res))!=0xFFFF) {
                    uint32_t d = PCI_CONFIG_GET_DEVICE(res);
                    HeaderGeneric *hdr = getHeader(bus, dev, func);
                    //Platform::video()->printf("Found device: %4x:%4x  class: %2x, %2x   %x %d\n",v,d,((HeaderCommon*)hdr)->classCode, ((HeaderCommon*)hdr)->subclass, ((HeaderCommon*)hdr)->headerType, PCI_CONFIG_IS_MULTI(((HeaderCommon*)hdr)->headerType));

                    if (m_verbose) Platform::video()->printf("PCI %2x:%2x.%d: %4x:%4x  class: %2x, %2x   headerType: %2x\n",bus,dev,func,v,d,
                        ((HeaderCommon*)hdr)->classCode, ((HeaderCommon*)hdr)->subclass,
                        ((HeaderCommon*)hdr)->headerType
                        );

                    if (PCI_CONFIG_IS_MULTI(((HeaderCommon*)hdr)->headerType)) {
                        multi = true;
                    }
                }
            }
        }
    }
}

PCI::DeviceIterator *PCI::startIter()
{
    return new DeviceIterator();
}

void PCI::endIter(DeviceIterator *iter)
{
    if (iter != nullptr) delete iter;
}

PCI::HeaderGeneric *PCI::findNextDevice(DeviceIterator *iter, uint8_t classcode, uint8_t subclass)
{
    if (iter == nullptr) return nullptr;
    while (iter->bus < 0x100) {
        while (iter->dev < 0x20) {
            bool multi = false;
            while (iter->func < 0x8) {
                if (iter->func > 0 && !multi) break;
                uint32_t res = getConfig(iter->bus, iter->dev, iter->func, 0);
                if (PCI_CONFIG_GET_VENDOR(res)!=0xFFFF) {
                    HeaderGeneric *hdr = getHeader(iter->bus, iter->dev, iter->func);
                    HeaderCommon *h = (HeaderCommon*)hdr;
                    if (h->classCode == classcode && h->subclass == subclass) {
                        return hdr;
                    }

                    if (PCI_CONFIG_IS_MULTI(((HeaderCommon*)hdr)->headerType)) {
                        multi = true;
                    }
                }
                iter->func++;
            }
            iter->func=0;
            iter->dev++;
        }
        iter->dev=0;
        iter->bus++;
    }

    return nullptr;
}

PCI::HeaderGeneric *PCI::findNextDeviceByVendor(DeviceIterator *iter, uint16_t vendor, uint16_t deviceid)
{
    if (iter == nullptr) return nullptr;
    while (iter->bus < 0x100) {
        while (iter->dev < 0x20) {
            bool multi = false;
            while (iter->func < 0x8) {
                if (iter->func > 0 && !multi) break;
                uint32_t res = getConfig(iter->bus, iter->dev, iter->func, 0);
                if (PCI_CONFIG_GET_VENDOR(res) != 0xFFFF) {
                    HeaderGeneric *hdr = getHeader(iter->bus, iter->dev, iter->func);
                    HeaderCommon *h = (HeaderCommon*)hdr;
                    if (h->vendor == vendor && h->device == deviceid) {
                        return hdr;
                    }

                    if (PCI_CONFIG_IS_MULTI(((HeaderCommon*)hdr)->headerType)) {
                        multi = true;
                    }
                }
                iter->func++;
            }
            iter->func=0;
            iter->dev++;
        }
        iter->dev=0;
        iter->bus++;
    }

    return nullptr;
}

PCI::HeaderGeneric *PCI::getCurrent(DeviceIterator *iter)
{
    uint32_t res = getConfig(iter->bus, iter->dev, iter->func, 0);

    if (PCI_CONFIG_GET_VENDOR(res) != 0xFFFF) {
        HeaderGeneric *hdr = getHeader(iter->bus, iter->dev, iter->func);
        return hdr;
    }
    return nullptr;
}
