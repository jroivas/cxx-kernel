#ifndef KERNEL_H
#define KERNEL_H

#include "video.h"
#define KERNEL_FREQUENCY 1020

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
