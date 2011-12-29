#ifndef _ATA_X86_H
#define _ATA_X86_H

#include "ata.h"

class ATAX86 : public ATA
{
public:
	ATAX86();
	~ATAX86();

protected:
	virtual void systemPortOut(uint32_t port, uint8_t val);
	virtual uint8_t systemPortIn(uint32_t port);
	virtual uint32_t systemPortInLong(uint32_t port);
};

#endif
