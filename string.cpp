#include "string.h"
#include "types.h"
#include "mm.h"

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


String::String()
{
	m_str = NULL;
}

String::String(const char *str)
{
	if (str!=NULL) {
		unsigned int l = length(str);
		m_str = (char*)kmalloc(l+1);
		Mem::copy((void*)m_str, (void*)str, l);
		m_str[l] = 0;
	} else m_str = NULL;
}

unsigned int String::length(const char *str)
{
	if (str==NULL) return 0;

	unsigned int cnt = 0;
	const char *tmp = str;
	while (tmp!=NULL) {
		cnt++;
		tmp++;
	}

	return cnt;
}

unsigned int String::length()
{
	return length(m_str);
}
