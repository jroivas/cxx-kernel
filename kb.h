#ifndef KB_H
#define KB_H

#include "regs.h"

class KB
{
public:
	static KB *get();

private:
	KB();
	void run(Regs *r);
	static void handler(Regs *r);
};

#endif
