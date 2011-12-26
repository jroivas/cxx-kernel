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
		uint32_t reg00;
		uint32_t reg04;
		uint32_t reg08;
		uint32_t reg0C;
		uint32_t reg10;
		uint32_t reg14;
		uint32_t reg18;
		uint32_t reg1C;
		uint32_t reg20;
		uint32_t reg24;
		uint32_t reg28;
		uint32_t reg2C;
		uint32_t reg30;
		uint32_t reg34;
		uint32_t reg38;
		uint32_t reg3C;
	};
	struct Header00 {
		struct HeaderCommon common;

		uint32_t baseAddress0;
		uint32_t baseAddress1;
		union {
			uint32_t baseAddress2;
			struct {
				uint8_t primaryBusNumber;
				uint8_t secondaryBusNumber;
				uint8_t subordinateBusNumber;
				uint8_t secondaryLatencyTimer;
			};
		};
		union {
			uint32_t baseAddress3;
			struct {
				uint8_t IObase;
				uint8_t IOlimit;
				uint16_t secondaryStatus;
			};
		};
		union {
			uint32_t baseAddress4;
			struct {
				uint16_t memoryBase;
				uint16_t memoryLimit;
			};
		};
		union {
			uint32_t baseAddress5;
			struct {
				uint16_t prefetcableMemoryBase;
				uint16_t prefetcableMemoryLimit;
			};
		};
		union {
			uint32_t cardbus;
			uint32_t prefetchableBaseUpper;
		};
		union {
			struct {
				uint16_t subsystemVendorID;
				uint16_t subsystemID;
			};
			uint32_t prefetchableLimitUpper;
		};
		union {
			uint32_t expansionROMAddress;
			struct {
				uint16_t IOBaseUpper;
				uint16_t IOLimitUpper;
			};
		};
		uint8_t caps;
		uint8_t res1;
		uint8_t res2;
		uint8_t res3;
		union {
			uint32_t res4;
			uint32_t expansionROMAddress;
		};
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
	}

	PCI();
	~PCI();
	uint32_t getVendor(uint32_t bus, uint32_t device);
	uint32_t getDevice(uint32_t bus, uint32_t device);
	uint32_t getHeaderType(uint32_t bus, uint32_t device);
	void scanDevices();

protected:
	uint32_t getConfig(uint32_t bus, uint32_t device, uint32_t func, uint32_t reg);
};

#endif
