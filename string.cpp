#include "string.h"
#include "types.h"

void *Mem::copy(void *dest, void *src, size_t size)
{
	char *d = (char*)dest;
	char *s = (char*)src;
	for (size_t i=0; i<size; i++) {
		*d=*s;
		d++;
		s++;
	}
	return dest;
}

void *Mem::set(void *s, unsigned char c, size_t size)
{
	unsigned char *d = (unsigned char*)s;
	for (size_t i=0; i<size; i++) {
		*d = c;
		d++;
	}
	return s;
}

void *Mem::setw(void *s, unsigned short c, size_t size)
{
	unsigned short *d = (unsigned short*)s;
	for (size_t i=0; i<size; i++) {
		*d = c;
		d++;
	}
	return s;
}

