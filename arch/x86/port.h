#ifndef PORT_H
#define PORT_H

class Port
{
public:
    Port() {}
    static unsigned char in(unsigned short port);
    static void out(unsigned short port, unsigned char out);
    static unsigned short inw(unsigned short port);
    static void outw(unsigned short port, unsigned short out);
    static unsigned int inl(unsigned short port);
    static void outl(unsigned short port, unsigned int out);
private:
};
#endif
