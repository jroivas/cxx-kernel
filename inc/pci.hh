#ifndef PCI_HH
#define PCI_HH

#include "bus.h"
#include "types.h"

class PCI : public Bus
{
public:
    struct HeaderCommon {
        //Common
        uint16_t vendor;
        uint16_t device;
        uint16_t command;
        uint16_t status;
        uint8_t revID;
        uint8_t progIF;
        uint8_t subclass;
        uint8_t classCode;
        uint8_t cacheLineSize;
        uint8_t latency;
        uint8_t headerType;
        uint8_t bist;

        // Refer to bus, dev and fucn
        uint32_t bus;
        uint32_t dev;
        uint32_t func;
    };
    struct HeaderGeneric {
        uint32_t reg[19];
    };
    struct Header00 {
        struct HeaderCommon common;

        uint32_t baseAddress0;
        uint32_t baseAddress1;
        uint32_t baseAddress2;
        uint32_t baseAddress3;
        uint32_t baseAddress4;
        uint32_t baseAddress5;
        uint32_t cardbus;
        uint16_t subsystemVendorID;
        uint16_t subsystemID;
        uint32_t expansionROMAddress;
        uint8_t caps;
        uint8_t res1;
        uint8_t res2;
        uint8_t res3;
        uint32_t res4;
        uint8_t interruptLine;
        uint8_t interruptPIN;
        uint8_t minGrant;
        uint8_t maxLatency;
    };
    struct Header01 {
        struct HeaderCommon common;

        uint32_t baseAddress0;
        uint32_t baseAddress1;
        uint8_t primaryBusNumber;
        uint8_t secondaryBusNumber;
        uint8_t subordinateBusNumber;
        uint8_t secondaryLatencyTimer;
        uint8_t IObase;
        uint8_t IOlimit;
        uint16_t secondaryStatus;
        uint16_t memoryBase;
        uint16_t memoryLimit;
        uint16_t prefetcableMemoryBase;
        uint16_t prefetcableMemoryLimit;
        uint32_t prefetchableBaseUpper;
        uint32_t prefetchableLimitUpper;
        uint16_t IOBaseUpper;
        uint16_t IOLimitUpper;
        uint8_t caps;
        uint8_t res1;
        uint8_t res2;
        uint8_t res3;
        uint32_t expansionROMAddress;
        uint8_t interruptLine;
        uint8_t interruptPIN;
        uint16_t bridgeControl;
    };
    class DeviceIterator;

    PCI();
    ~PCI();
    bool isAvailable();
    HeaderGeneric *getHeader(uint32_t bus, uint32_t device, uint32_t func);
    void setHeader(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg, uint8_t val);
    bool isDevice(uint32_t bus, uint32_t device, uint32_t func);
    void scanDevices();
    void setVerbose(bool verb=true)
    {
        m_verbose = verb;
    }

    DeviceIterator *startIter();
    HeaderGeneric *findNextDevice(DeviceIterator *iter, uint8_t classcode, uint8_t subclass);
    HeaderGeneric *findNextDeviceByVendor(DeviceIterator *iter, uint16_t vendor, uint16_t deviceid);
    HeaderGeneric *getCurrent(DeviceIterator *iter);
    void endIter(DeviceIterator *iter);
    void set(DeviceIterator *iter, uint32_t reg, uint8_t val);

    uint32_t readValue32(HeaderGeneric *hc, uint32_t addr);

protected:
    uint32_t getVendor(uint32_t bus, uint32_t device, uint32_t func);
    uint32_t getDevice(uint32_t bus, uint32_t device, uint32_t func);
    uint32_t getPciAddr(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg);
    uint32_t getConfig(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg);
    void setConfig(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg, uint32_t val);
    virtual void systemPut(uint32_t val) = 0;
    virtual void systemPutData(uint8_t val) = 0;
    virtual uint32_t systemGet() = 0;
    bool m_verbose;
};

#endif
