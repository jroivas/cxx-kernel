#ifndef KB_H
#define KB_H

#include "regs.h"

class KB
{
public:
	static KB *get();

protected:
	KB() { }
	static int handler(Regs *r);
	virtual void run(Regs *r) = 0;
};

#endif
