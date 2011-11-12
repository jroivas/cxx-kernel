#ifndef BITS_H
#define BITS_H

#include "types.h"

class Bits
{
public:
	Bits(uint32_t cnt);
	void clearAll();
	bool set(uint32_t i);
	bool clear(uint32_t i);
	bool isSet(uint32_t i);
	uint32_t findUnset(bool *ok);

private:
	uint32_t *bits;
	uint32_t m_len;
	uint32_t m_cnt;
};

#endif
