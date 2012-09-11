#ifndef GENERAL_ARMFB
#define GENERAL_ARMFB

#ifdef PLATFORM_qemu
#include "qemu/armfb.h"
#endif
#ifdef PLATFORM_pi
#include "pi/armfb.h"
#endif

#endif
