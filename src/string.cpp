#include "string.hh"
#include "types.h"
#include "mm.h"
#include "platform.h"

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
    if (dest == nullptr || src == nullptr) return nullptr;
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

int Mem::cmp(const void *s1, const void *s2, size_t size)
{
    const char *a = (const char*)s1;
    const char *b = (const char*)s2;

    if ((!a && !b) || (!a && b))
        return -1;
    if (a && !b)
        return 1;

    while (size--) {
        if (*a < *b)
            return -1;
        else if (*a > *b)
            return 1;
        a++;
        b++;
    }
    return 0;
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


String::String()
    : m_length(0),
    m_str(nullptr),
    m_null(0)
{
}

String::String(const char *str)
    : m_null(0)
{
    init(str);
}

String::String(const String &str)
    : m_null(0)
{
    init(str.m_str, str.m_length);
}

String::String(char c)
    : m_null(0)
{
    m_length = 1;
    m_str = (char*)MM::instance()->alloc(m_length + 1);
    m_str[0] = c;
    m_str[m_length] = 0;
}

String::~String()
{
    if (m_str != nullptr) {
        MM::instance()->free(m_str);
        m_str = nullptr;
    }
}

void String::init(const char *str, ssize_t len)
{
    if (str != nullptr) {
        if (len >= 0) {
            m_length = len;
        } else {
            m_length = length(str);
        }
        m_str = (char*)MM::instance()->alloc(m_length + 1);
        Mem::copy((void*)m_str, (void*)str, m_length);
        m_str[m_length] = 0;
    } else {
        m_str = nullptr;
    }
}

size_t String::length(const char *str)
{
    if (str == nullptr) return 0;

    unsigned int cnt = 0;
    const char *tmp = str;
    while (*tmp != 0) {
        cnt++;
        tmp++;
    }

    return cnt;
}

size_t String::length() const
{
    return m_length;
}

const char &String::at(size_t pos) const
{
    if (pos >= m_length) {
        // TODO FIXME Exception
        return m_null;
    }
    return m_str[pos];
}

char &String::at(size_t pos)
{
    if (pos >= m_length) {
        // TODO FIXME Exception
        return m_null;
    }
    return m_str[pos];
}

String &String::append(const char* str, size_t len)
{
    size_t new_length = m_length + len;

    char *tmp = (char*)MM::instance()->realloc(m_str, new_length + 1);
    Mem::move((void*)(tmp + m_length), (void*)str, len);

    m_str = tmp;
    m_length = new_length;
    m_str[m_length] = 0;

    return *this;
}

String &String::append(const String &str)
{
    return append(str.c_str(), str.m_length);
}

String &String::append(const char *str)
{
    return append(str, length(str));
}

String &String::operator=(const String& str)
{
    if (this != &str) {
        init(str.m_str, str.m_length);
    }
    return *this;
}

String &String::operator+=(const String& str)
{
    return append(str.c_str(), str.m_length);
}

String &String::operator+=(const char *str)
{
    return append(String(str));
}

String &String::operator+=(char c)
{
    char *tmp = (char*)MM::instance()->realloc(m_str, m_length + 2);
    tmp[m_length] = c;

    m_str = tmp;
    ++m_length;
    m_str[m_length] = 0;

    return *this;
}

bool String::operator==(const String &other) const
{
    if (other.length() != m_length) return false;

    for (size_t i = 0; i < m_length; ++i) {
        if (m_str[i] != other.m_str[i]) return false;
    }

    return true;
}

bool String::startsWith(const String &other) const
{
    size_t len = other.m_length;
    if (m_length < len) {
        return false;
    }

    for (size_t i = 0; i < len; ++i) {
        if (m_str[i] != other.m_str[i]) return false;
    }

    return true;
}

String String::right(size_t index) const
{
    String res;

    for (size_t i = index; i < m_length; ++i) {
        res += (char)m_str[i];
    }

    return res;
}

String String::left(size_t index) const
{
    String res;

    size_t cnt = 0;
    for (size_t i = 0; i < m_length; ++i) {
        res += (char)m_str[i];

        if (cnt >= index) break;
        ++index;
    }

    return res;
}
