#ifndef PORT_H
#define PORT_H

class Port
{
public:
	Port() {}
	static unsigned char in(unsigned short port);
	static void out(unsigned short port, unsigned char out);
private:
};
#endif
