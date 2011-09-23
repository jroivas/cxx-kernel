#include "port.h"

unsigned char Port::in(unsigned short port)
{
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}

void Port::out(unsigned short port, unsigned char data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}
