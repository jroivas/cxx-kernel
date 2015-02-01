#ifndef GENERAL_ARMFB
#define GENERAL_ARMFB

#if defined(PLATFORM_qemu)
#include "qemu/armfb.h"
#elif defined(PLATFORM_pi)
#include "pi/armfb.h"
#else
#error "Please define ARM subplatform: qemu or pi"
#endif

#endif
