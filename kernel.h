#ifndef KERNEL_H
#define KERNEL_H

#include "video.h"

class Kernel
{
public:
	Kernel();
	~Kernel();
	int run();
private:
	Video *video;
};

#endif
