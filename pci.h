#ifndef PCI_H
#define PCI_H

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
	};
	struct HeaderGeneric {
		uint32_t reg[16];
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

	PCI();
	~PCI();
	bool isAvailable();
	HeaderGeneric *getHeader(uint32_t bus, uint32_t device);
	bool isDevice(uint32_t bus, uint32_t device);
	void scanDevices();

protected:
	uint32_t getVendor(uint32_t bus, uint32_t device);
	uint32_t getDevice(uint32_t bus, uint32_t device);
	uint32_t getConfig(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg);
	virtual void systemPut(uint32_t val) = 0;
	virtual uint32_t systemGet() = 0;
};

#endif
