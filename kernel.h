#ifndef KERNEL_H
#define KERNEL_H

#include "video.h"
#include "states.h"
#include "arch/platform.h"
#define KERNEL_FREQUENCY 1020

class Kernel
{
public:
	Kernel();
	~Kernel();
	int run();
private:
	Video *video;
	Platform *platform;
};

#endif
