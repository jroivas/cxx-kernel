#include "bits.h"
#define BIT_CONVERT (8*4)

Bits::Bits(uint32_t cnt)
{
	m_cnt = cnt;
	m_len = cnt/BIT_CONVERT;
	if (cnt%BIT_CONVERT!=0) m_len++;
	bits = new uint32_t[m_len];
	m_last = 0;
	clearAll();
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

	uint32_t n = i/BIT_CONVERT;
	uint32_t m = i%BIT_CONVERT;

	bits[n] |= (1<<m);

	return true;
}

bool Bits::clear(uint32_t i)
{
	if (i>=m_cnt) return false;

	uint32_t n = i/BIT_CONVERT;
	uint32_t m = i%BIT_CONVERT;

	bits[n] &= ~(1<<m);
	m_last = 0;

	return true;
}

bool Bits::isSet(uint32_t i)
{
	if (i>=m_cnt) return false;

	uint32_t n = i/BIT_CONVERT;
	uint32_t m = i%BIT_CONVERT;

	if (bits[n] & (1<<m)) return true;
	return false;
}

uint32_t Bits::findUnset(bool *ok)
{
	for (uint32_t i=m_last; i<m_len; i++) {
		// Check if we have unset bits
		if (~bits[i] != 0) {
			for (uint32_t j=0; j<32; j++) {
				if ((bits[i] & (1<<j))==0) {
					if (ok!=NULL) *ok = true;
					m_last = i;
					return i*BIT_CONVERT+j;
				}
			}
		}
#if 0
		if (m_last>0 && i>=m_len) {
			m_last = 0;
			i = 0;
		}
#endif
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