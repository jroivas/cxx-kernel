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

unsigned short Port::inw(unsigned short port)
{
	unsigned short rv;
	__asm__ __volatile__ ("inw %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}

void Port::outw(unsigned short port, unsigned short data)
{
	__asm__ __volatile__ ("outw %1, %0" : : "dN" (port), "a" (data));
}

unsigned int Port::inl(unsigned short port)
{
	unsigned int rv;
	__asm__ __volatile__ ("inl %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}

void Port::outl(unsigned short port, unsigned int data)
{
	__asm__ __volatile__ ("outl %1, %0" : : "dN" (port), "a" (data));
}
