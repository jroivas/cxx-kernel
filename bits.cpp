#include "bits.h"

Bits::Bits(uint32_t cnt)
{
	m_cnt = cnt;
	m_len = cnt/(8*4);
	if (cnt%(8*4)!=0) m_len++;
	bits = new uint32_t[m_len];
	m_last = 0;
}

void Bits::clearAll()
{
	for (uint32_t i=0; i<m_len; i++) {
		bits[i] = 0;
	}
}

bool Bits::set(uint32_t i)
{
	if (i>=m_cnt) return false;

	uint32_t n = i/(8*4);
	uint32_t m = i%(8*4);

	bits[n] |= (1<<m);

	return true;
}

bool Bits::clear(uint32_t i)
{
	if (i>=m_cnt) return false;

	uint32_t n = i/(8*4);
	uint32_t m = i%(8*4);

	bits[n] &= ~(1<<m);
	m_last = 0;

	return true;
}

bool Bits::isSet(uint32_t i)
{
	if (i>=m_cnt) return false;

	uint32_t n = i/(8*4);
	uint32_t m = i%(8*4);

	if (bits[n] & (1<<m)) return true;
	return false;
}

uint32_t Bits::findUnset(bool *ok)
{
	for (uint32_t i=m_last; i<m_len; i++) {
		// Check if we have unset bits
		if (~bits[i] != 0) {
			for (uint32_t j=0; j<32; j++) {
				if (!bits[i] & (1<<j)) {
					if (ok!=NULL) *ok = true;
					m_last = i;
					return i*4*8*j;
				}
			}
		}
	}
	if (ok!=NULL) *ok = false;
	return 0;
}

void Bits::copyFrom(uint32_t *from)
{
	for (uint32_t i=0; i<m_len; i++) {
		bits[i] = from[i];
	}
}
