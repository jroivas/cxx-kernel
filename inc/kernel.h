#ifndef KERNEL_H
#define KERNEL_H

#include "video.h"
#include "states.h"
#include "syscall.hh"
#include "arch/platform.h"

/* Kernel HZ, means how often timer will hit on a second.
   Bigger number means better responsiveness,
   but causes more interrupt.

   On server loads lower is better,
   on desktop higher.
 */
#define KERNEL_FREQUENCY 100
//#define KERNEL_FREQUENCY 300
//#define KERNEL_FREQUENCY 1000

class Kernel
{
public:
    Kernel();
    ~Kernel();
    int run();
private:
    Video *video;
    Platform *platform;
    VFS *vfs;
    PCI *pcidev;
    SysCall *sys;

    void initVideo();
    void initFileSystem();
    void initPCI();
    void initATA(Filesystem *cfs);
    void initVirtualDisc();
    int initFrameBuffer();
    void drawTestData();
    void initSysCall();
    void startProcessManager();
};

#endif
