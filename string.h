#ifndef STRING_H
#define STRING_H

#include "types.h"

class Mem
{
public:
	Mem() { }
	static void *copy(void *dest, void *src, size_t size);
	static void *set(void *s, unsigned char c, size_t size);
	static void *setw(void *s, unsigned short c, size_t size);
private:
};

#endif
