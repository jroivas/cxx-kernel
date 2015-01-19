#include "string.hh"
#include "types.h"
#include "mm.h"

void *Mem::copy(void *dest, const void *src, size_t size)
{
    if (dest == src) return dest;

    char *d = (char*)dest;
    char *s = (char*)src;
    for (size_t i = 0; i < size; i++) {
        *d++ = *s++;
    }
    return dest;
}

void *Mem::move(void *dest, const void *src, size_t size)
{
    if (dest == NULL || src == NULL) return NULL;
    if (dest == src) return dest;

    if (dest > src) {
        char *d = ((char*)dest) + size - 1;
        char *s = ((char*)src) + size - 1;
        for (size_t i = 0; i < size; i++) {
            *d = *s;
            d--;
            s--;
        }
    } else return Mem::copy(dest,src,size);

    return dest;
}

void *Mem::set(void *s, unsigned char c, size_t size)
{
    unsigned char *d = (unsigned char*)s;
    for (size_t i = 0; i < size; i++) {
        *d++ = c;
    }
    return s;
}

void *Mem::setw(void *s, unsigned short c, size_t size)
{
    unsigned short *d = (unsigned short*)s;
    for (size_t i = 0; i < size; i++) {
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
    if (str != NULL) {
        unsigned int l = length(str);
        m_str = (char*)MM::instance()->alloc(l + 1);
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

extern "C" void *memcpy(void *dest, const void *src, unsigned int n)
{
    return Mem::copy(dest, (void*)src, n);
}

extern "C" void *memset(void *s, int c, size_t n)
{
    return Mem::set(s, c, n);
}

extern "C" void *memmove(void *dest, const void *src, size_t n)
{
    return Mem::move(dest, (void*)src, n);
}
