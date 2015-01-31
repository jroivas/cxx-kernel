#ifndef STRING_HH
#define STRING_HH

#include "types.h"

#ifdef __cplusplus

class Mem
{
public:
    Mem() { }
    static void *copy(void *dest, const void *src, size_t size);
    static void *move(void *dest, const void *src, size_t size);
    static void *set(void *s, unsigned char c, size_t size);
    static void *setw(void *s, unsigned short c, size_t size);
private:
};

class String
{
public:
    String();
    String(const char *str);
    String(const String &str);
    String(char c);
    ~String();

    String &operator=(const String& str);
    String &append(const String &str);
    String &append(const char *str);
    String &append(const char c);
    String &operator+=(const String& str);
    String &operator+=(const char *str);
    String &operator+=(char c);

    bool operator==(const String &other) const;

    bool startsWith(const String &other) const;
    String right(size_t index) const;
    String left(size_t index) const;

    size_t length() const;
    size_t size() const
    {
        return length();
    }
    static size_t length(const char *str);

    const char &at(size_t pos) const;
    char &at(size_t pos);
    const char& operator[](size_t pos) const
    {
        return at(pos);
    }
    char& operator[](size_t pos)
    {
        return at(pos);
    }

    const char *c_str() const
    {
        return m_str;
    }

private:
    void init(const char *str, ssize_t len=-1);
    String &append(const char* str, size_t len);

    size_t m_length;
    char *m_str;
    char m_null;
};
#endif

#endif
